#include "NLMDenoiserDefinitions.hpp"
#include "NLMDenoiserPlugin.hpp"
#include "imageUtils/noiseAnalysis.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/IProgress.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>
#include <terry/channel.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cmath>
#include <vector>
#include <sstream>

namespace tuttle
{
namespace plugin
{
namespace nlmDenoiser
{

namespace bgil = boost::gil;

template <class View>
NLMDenoiserProcess<View>::NLMDenoiserProcess(NLMDenoiserPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
    _paramRedStrength = instance.fetchDoubleParam(kParamRedStrength);
    _paramGreenStrength = instance.fetchDoubleParam(kParamGreenStrength);
    _paramBlueStrength = instance.fetchDoubleParam(kParamBlueStrength);

    _paramRedGrainSize = instance.fetchDoubleParam(kParamRedGrainSize);
    _paramGreenGrainSize = instance.fetchDoubleParam(kParamGreenGrainSize);
    _paramBlueGrainSize = instance.fetchDoubleParam(kParamBlueGrainSize);

    _paramPatchRadius = instance.fetchIntParam(kParamPatchRadius);
    _paramRegionRadius = instance.fetchIntParam(kParamRegionRadius);
    _paramDepth = instance.fetchIntParam(kParamDepth);
    _paramPreBlurring = instance.fetchDoubleParam(kParamPreBlurring);

    _paramOptimized = instance.fetchBooleanParam(kParamOptimization);
}

template <class View>
NLMDenoiserProcess<View>::~NLMDenoiserProcess()
{
}

template <class View>
void NLMDenoiserProcess<View>::addFrame(const OfxRectI& dBounds, const int dstBitDepth, const int dstComponents,
                                        const double time, const int z)
{
    // Fetch main input image
    TUTTLE_LOG_INFO("NLMDenoiserProcess<View>::addFrame time:" << time);
    OFX::Image* img = _plugin._clipSrc->fetchImage(time);
    if(!img)
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    _srcImgs.push_back(img);
    OfxRectI bounds = img->getBounds();
    const int w = bounds.x2 - bounds.x1;
    const int h = bounds.y2 - bounds.y1;

    memcpy(&_upScaledBounds, &bounds, sizeof(OfxRectI));

    // Build views
    _srcViews.push_back(bgil::interleaved_view(w, h, (Pixel*)img->getPixelData(), img->getRowDistanceBytes()));

    // Make sure bit depths are same
    OFX::EBitDepth srcBitDepth = img->getPixelDepth();
    OFX::EPixelComponent srcComponents = img->getPixelComponents();

    // See if they have the same bit depths
    if(srcBitDepth != dstBitDepth || srcComponents != dstComponents)
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch());
    }
}

template <class View>
void NLMDenoiserProcess<View>::setup(const OFX::RenderArguments& args)
{
    const int depth = _paramDepth->getValue();

    _srcViews.clear();
    _srcImgs.clear();

    this->_dst.reset(_plugin._clipDst->fetchImage(args.time));
    // Fetch output image
    if(!this->_dst.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    const OfxRectI dBounds = this->_dst->getBounds();

    const OFX::EBitDepth dstBitDepth = this->_dst->getPixelDepth();
    const OFX::EPixelComponent dstComponents = this->_dst->getPixelComponents();

    this->_dstView = bgil::interleaved_view(dBounds.x2 - dBounds.x1, dBounds.y2 - dBounds.y1,
                                            (Pixel*)this->_dst->getPixelData(), this->_dst->getRowDistanceBytes());

    // Get render frame range
    const OfxRangeD clipFullRange = _plugin._clipSrc->getFrameRange();
    OfxRangeD requestedRange;
    requestedRange.min = args.time - depth;
    requestedRange.max = args.time + depth;
    OfxRangeD realRange;
    realRange.min = std::max(requestedRange.min, clipFullRange.min);
    realRange.max = std::min(requestedRange.max, clipFullRange.max);

    int i = 0;
    addFrame(dBounds, dstBitDepth, dstComponents, (int)(args.time), i++);

    for(OfxTime t = args.time - 1; t >= realRange.min; --t)
    {
        TUTTLE_LOG_VAR2(TUTTLE_INFO, args.time, t);
        addFrame(dBounds, dstBitDepth, dstComponents, t, i++);
    }

    for(OfxTime t = args.time + 1; t <= realRange.max; ++t)
    {
        TUTTLE_LOG_VAR2(TUTTLE_INFO, args.time, t);
        addFrame(dBounds, dstBitDepth, dstComponents, t, i++);
    }
}

template <class View>
void NLMDenoiserProcess<View>::preProcess()
{
    // Initialize progress bar
    if(_paramOptimized->getValue())
    {
        const int min_xpi = std::min((int)_paramRegionRadius->getValue() * 2, (int)_srcViews[0].width()),
                  min_ypi = std::min((int)_paramRegionRadius->getValue() * 2, (int)_srcViews[0].height());
        std::stringstream msg;
        msg << "NL-Means algorithm in progress (automatic bandwidth = " << computeBandwidth() << ").";
        this->progressBegin(_paramDepth->getValue() * min_xpi * min_ypi, msg.str());
    }
    else
    {
        this->progressBegin((int)((_srcViews[0].width() * this->_renderArgs.renderScale.x) *
                                  (_srcViews[0].height() * this->_renderArgs.renderScale.y) /
                                  _plugin._clipSrc->getPixelAspectRatio()),
                            "NL-Means algorithm in progress");
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void NLMDenoiserProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    NlmParams params;
    // Change reference point
    params.mix[0] = (float)_paramRedStrength->getValue();
    params.mix[1] = (float)_paramGreenStrength->getValue();
    params.mix[2] = (float)_paramBlueStrength->getValue();
    params.mix[2] = 1.0;

    params.bws[0] = (float)_paramRedGrainSize->getValue();
    params.bws[1] = (float)_paramGreenGrainSize->getValue();
    params.bws[2] = (float)_paramBlueGrainSize->getValue();
    params.bws[3] = 1.0;

    params.patchRadius = _paramPatchRadius->getValue();
    params.regionRadius = _paramRegionRadius->getValue();
    params.preBlurring = (float)_paramPreBlurring->getValue();

    // Destination subview cropped by the procwindow
    View subDst = bgil::subimage_view(this->_dstView, procWindowRoW.x1 - this->_renderArgs.renderWindow.x1,
                                      procWindowRoW.y1 - this->_renderArgs.renderWindow.y1,
                                      procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1);
    nlMeans(subDst, procWindowRoW, params);
}

template <class View>
double NLMDenoiserProcess<View>::computeBandwidth()
{
    double bandwidth = 0.0;
    // Assume that we work in RGB
    const int nc = 3;
    double np = (2 * _paramPatchRadius->getValue()) * (2 * _paramPatchRadius->getValue()) * nc;
    if(np < 100)
        bandwidth =
            ((((((1.1785e-12 * np - 5.1827e-10) * np + 9.5946e-08) * np - 9.7798e-06) * np + 6.0756e-04) * np - 0.0248) *
                 np +
             1.9203) *
                np +
            7.9599;
    else
        bandwidth = (-7.2611e-04 * np + 1.3213) * np + 15.2726;

    return bandwidth / _paramDepth->getValue();
}

/**
 * @brief Function called to apply nl-means denoising
 *
 * @param[out] dst  Destination image view
 * @param[in] procWindow
 * @param[in] params
 *
 */
template <class View>
void NLMDenoiserProcess<View>::nlMeans(View& dst, const OfxRectI& procWindow, const NlmParams& params)
{
    using namespace boost::gil;
    using namespace terry;

    typedef typename rgba32f_view_t::x_iterator WeightIt;
    typedef typename View::x_iterator x_iterator;
    typedef typename View::locator Locator;

    const int w = dst.width();
    const int h = dst.height();

    static const int nc = num_channels<Pixel>::value;
    boost::array<float, nc> mix;

    for(std::size_t i = 0; i < mix.size(); ++i)
    {
        mix[i] = params.mix[i] * channel_traits<Channel>::max_value();
    }

    std::vector<View> subSrcViews;
    typename std::vector<View>::iterator it;
    const int margin = params.regionRadius + params.patchRadius;

    // Upscale process window
    OfxRectI upscaledProcWindow, tUpscaledProcWindow;
    upscaledProcWindow.x1 = procWindow.x1 - margin - 1;
    upscaledProcWindow.y1 = procWindow.y1 - margin - 1;
    upscaledProcWindow.x2 = procWindow.x2 + margin + 1;
    upscaledProcWindow.y2 = procWindow.y2 + margin + 1;
    upscaledProcWindow = rectanglesIntersection(upscaledProcWindow, _upScaledBounds);

    // Translate
    tUpscaledProcWindow.x1 = upscaledProcWindow.x1 - _upScaledBounds.x1;
    tUpscaledProcWindow.y1 = upscaledProcWindow.y1 - _upScaledBounds.y1;
    tUpscaledProcWindow.x2 = upscaledProcWindow.x2 - _upScaledBounds.x1;
    tUpscaledProcWindow.y2 = upscaledProcWindow.y2 - _upScaledBounds.y1;

    // Create up-scaled-proc-windowed subviews sequence
    for(it = _srcViews.begin(); it != _srcViews.end(); ++it)
    {
        subSrcViews.push_back(subimage_view(*it, tUpscaledProcWindow.x1, tUpscaledProcWindow.y1,
                                            tUpscaledProcWindow.x2 - tUpscaledProcWindow.x1,
                                            tUpscaledProcWindow.y2 - tUpscaledProcWindow.y1));
    }

    // Allocate average buffers (assimilate this two buffers as 2D float buffers, not images !)
    rgba32f_image_t weight_cumul(w, h); /// @todo: use memory allocated by host using memorySuite
    rgba32f_image_t weight_norm(w, h);
    rgba32f_view_t view_wc(view(weight_cumul));
    rgba32f_view_t view_norm(view(weight_norm));

    // Initialize averaging buffer to 0
    fill_black(view_wc);
    fill_black(view_norm);

    // Bugs bunny is here
    OfxRectI nProcWindow;
    nProcWindow.x1 = (procWindow.x1 - upscaledProcWindow.x1);
    nProcWindow.y1 = (procWindow.y1 - upscaledProcWindow.y1);
    nProcWindow.x2 = nProcWindow.x1 + w;
    nProcWindow.y2 = nProcWindow.y1 + h;

    computeWeights(subSrcViews, nProcWindow, view_wc, view_norm, params);

    if(!_plugin.abort())
    {
        View procView = subimage_view(subSrcViews[0], nProcWindow.x1, nProcWindow.y1, w, h);
        for(int yj = 0; yj < h; ++yj)
        {
            WeightIt wcIter = view_wc.row_begin(yj);
            WeightIt wnIter = view_norm.row_begin(yj);
            x_iterator src_it = procView.row_begin(yj);
            x_iterator dst_it = dst.row_begin(yj);
            for(int xj = 0; xj < w; ++xj)
            {
                // Final estimate
                for(int v = 0; v < nc; ++v)
                {
                    (*dst_it)[v] = (((*wcIter)[v] * mix[v] + 1.0f * (*src_it)[v]) / ((*wnIter)[v] * mix[v] + 1.0f));
                }

                // Fill alpha
                assign_channel_if_exists_t<Pixel, alpha_t>()(*src_it, *dst_it);

                ++src_it;
                ++dst_it;
                ++wcIter;
                ++wnIter;
            }
            this->progressForward(w);
        }
    }
}

template <class View>
void NLMDenoiserProcess<View>::computeWeights(const std::vector<View>& srcViews, const OfxRectI& procWindow,
                                              bgil::rgba32f_view_t& view_wc, bgil::rgba32f_view_t& view_norm,
                                              const NlmParams& params)
{
    typedef typename View::x_iterator sIterator;
    typedef typename bgil::rgba32f_view_t::x_iterator wIterator;
    typedef typename bgil::channel_type<View>::type dpix_t;
    typedef typename View::locator Loc;
    typedef typename bgil::rgba32f_view_t::locator WLoc;

    const int patchRadius = params.patchRadius;
    const int depth = srcViews.size();

    const int wi = srcViews[0].width();
    const int hi = srcViews[0].height();

    // Noise variance estimation
    const double nv = imageUtils::noise_variance(srcViews[0]);
    const double sigma = std::sqrt(nv < 0 ? 0 : nv);
    Loc loc1, loc2;
    WLoc wcLoc, wnLoc;

    // Optimisation based on: AN IMPROVED NON-LOCAL DENOISING ALGORITHM, LNLA 2008
    // Define the size of the neighborhood
    const int min_xpi = std::min(params.regionRadius, wi / 2);
    const int min_ypi = std::min(params.regionRadius, hi / 2);

    static const int nc = boost::mpl::min<boost::mpl::int_<3>, typename bgil::num_channels<Pixel>::type>::type::value;
    boost::array<float, nc> bws;
    for(std::size_t i = 0; i < bws.size(); ++i)
    {
        bws[i] = params.bws[i];
    }
    int lbound, hbound;
    // [Kervrann] notations
    std::vector<double> h1(nc);
    std::vector<double> h2(nc);

    for(int i = 0; i < nc; ++i)
    {
        if(bws[i] < 0)
            bws[i] = (float)computeBandwidth();
        h1[i] = bws[i] * sigma;
        h2[i] = 1.0 / (h1[i] * h1[i]);
    }

    double abs_e, eucl_dist, weigth, e;

    // For zi (displacment)
    for(int zi = 0; zi < depth; ++zi)
    {
        // For yi (displacment)
        for(int yi = -min_ypi; yi <= min_ypi; ++yi)
        {
            // For xi (displacment)
            for(int xi = -min_xpi; xi <= min_xpi; ++xi)
            {
                // If not 0 displacment
                if(xi != 0 || yi != 0)
                {
                    const int xl = xi < 0 ? std::abs(xi) : 0;
                    const int xh = wi + xi > wi ? wi - xi : wi;
                    const int yl = yi < 0 ? std::abs(yi) : 0;
                    const int yh = hi + yi > hi ? hi - yi : hi;

                    // For yj
                    for(int yj = yl; yj < yh; ++yj)
                    {
                        // Initialize patch euclidian distance
                        eucl_dist = 0.0f;
                        int j = yj + yi;
                        // Horizontal & vertical averaging bounds
                        if(yi >= 0)
                        {
                            lbound = -patchRadius + yj < 0 ? std::max(-yj, -patchRadius) : -patchRadius;
                            hbound = lbound + (patchRadius * 2 + 1);
                            if(hbound + j > hi)
                                hbound = std::min(hi - j, patchRadius);
                        }
                        else
                        {
                            hbound = patchRadius + yj > hi ? std::min(patchRadius, (hi - yj)) : patchRadius;
                            lbound = hbound - (patchRadius * 2 + 1);
                            if(lbound + j < 0)
                                lbound = 0;
                        }

                        // Warmup: initial average accumulation
                        int xl_bound = std::max(xl - patchRadius, 0);
                        int xr_bound = std::min(wi, xl_bound + patchRadius * 2);
                        loc1 = srcViews[zi].xy_at(xi, j);
                        loc2 = srcViews[0].xy_at(0, yj);
                        for(int xj = xl_bound; xj < xr_bound; ++xj)
                        {
                            // following "if" is bad but simplify the code
                            if((xj + xi) >= 0)
                            {
                                for(int k = lbound; k < hbound; ++k)
                                {
                                    for(int v = 0; v < nc; ++v)
                                    {
                                        e = loc1(xj, k)[v] - loc2(xj, k)[v];
                                        eucl_dist += e * e;
                                    }
                                }
                            }
                        }

                        // For xj
                        for(int xj = xl; xj < xh; ++xj)
                        {
                            int i = xj + xi;

                            loc1 = srcViews[zi].xy_at(i, j);
                            loc2 = srcViews[0].xy_at(xj, yj);

                            // 2D centered patch sliding average
                            if(patchRadius + i < wi && patchRadius + xj < wi)
                            {
                                for(int k = lbound; k < hbound; ++k)
                                {
                                    for(int v = 0; v < nc; ++v)
                                    {
                                        e = loc1(patchRadius, k)[v] - loc2(patchRadius, k)[v];
                                        eucl_dist += e * e;
                                    }
                                }
                            }

                            if(xj - patchRadius - 1 >= 0 && i - patchRadius - 1 >= 0)
                            {
                                for(int k = lbound; k < hbound; ++k)
                                {
                                    for(int v = 0; v < nc; ++v)
                                    {
                                        e = loc1(-patchRadius - 1, k)[v] - loc2(-patchRadius - 1, k)[v];
                                        eucl_dist -= e * e;
                                    }
                                }
                            }

                            // Symetric weigthening will be computed
                            bool w1Pass = (zi == 0 && i >= procWindow.x1 && i < procWindow.x2 && j >= procWindow.y1 &&
                                           j < procWindow.y2);

                            // Weigthening will be computed
                            bool w2Pass =
                                (xj >= procWindow.x1 && xj < procWindow.x2 && yj >= procWindow.y1 && yj < procWindow.y2);

                            // Weight computation (Modified Bisquare weightening function)
                            if(w1Pass || w2Pass)
                            {
                                wcLoc = view_wc.xy_at(xj - procWindow.x1, yj - procWindow.y1);
                                wnLoc = view_norm.xy_at(xj - procWindow.x1, yj - procWindow.y1);

                                abs_e = std::abs(eucl_dist);
                                for(int v = 0; v < nc; ++v)
                                {
                                    if(abs_e <= h1[v])
                                    {
                                        weigth = 1.0 - (abs_e * abs_e * h2[v]);
                                        // Powerize to 8
                                        weigth *= weigth;
                                        weigth *= weigth;
                                        weigth *= weigth;

                                        // Weight accumulation
                                        if(w1Pass)
                                        {
                                            wcLoc(xi, yi)[v] += weigth * loc2(0, 0)[v];
                                            wnLoc(xi, yi)[v] += weigth;
                                        }
                                        if(w2Pass)
                                        {
                                            // Symmetry
                                            wcLoc(0, 0)[v] += weigth * loc1(0, 0)[v];
                                            wnLoc(0, 0)[v] += weigth;
                                        }
                                    }
                                }
                            }
                        }                    // End for xj
                    }                        // End for yj
                }                            // End if not 0 displacment
                if(this->progressForward(1)) /// @todo what is the step here ??
                    return;
            } // End for xi (displacment)
        }     // End for yi (displacment)
    }         // End for zi (displacment)
}
}
}
}
