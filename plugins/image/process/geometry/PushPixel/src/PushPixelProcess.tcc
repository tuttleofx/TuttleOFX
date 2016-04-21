#include "PushPixelPlugin.hpp"
#include "PushPixelDefinitions.hpp"
#include "PushPixelProcess.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>
#include <terry/channel_view.hpp>
#include <terry/filter/motionVectors.hpp>
#include <terry/filter/convolve.hpp>
#include <terry/sampler/all.hpp>

#include <boost/gil/utilities.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

template <class View>
PushPixelProcess<View>::PushPixelProcess(PushPixelPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
    _clipMask = effect.fetchClip(kClipMask);
    _clipMaskConnected = _clipMask->isConnected();

    if(_clipMaskConnected && (_clipMask->getPixelComponents() != this->_clipSrc->getPixelComponents()))
        BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user("Can't mix different input Pixel Components."));
}

template <class View>
void PushPixelProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    // mask view
    if(_clipMaskConnected)
    {
        this->_mask.reset(_clipMask->fetchImage(args.time));
        if(!this->_mask.get())
            BOOST_THROW_EXCEPTION(exception::ImageNotReady());
        if(this->_mask->getRowDistanceBytes() <= 0)
            BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        this->_maskView = this->getView(this->_mask.get(), _clipMask->getPixelRod(args.time, args.renderScale));

        if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
        {
            // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
            _maskPixelRod = _clipMask->getPixelRod(args.time, args.renderScale);
        }
        else
        {
            _maskPixelRod = _mask->getRegionOfDefinition();
        }
    }

    _params = _plugin.getProcessParams(args.renderScale);

    /*
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_renderArgs.fieldToRender );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_renderArgs.renderScale );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_renderArgs.renderWindow );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_renderArgs.time );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_srcPixelRod );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_dstPixelRod );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_srcView.width(), this->_srcView.height() );
    TUTTLE_LOG_VAR( TUTTLE_INFO, this->_dstView.width(), this->_dstView.height() );
    */
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void PushPixelProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::filter;

    typedef View MaskView; /// @todo add a MaskView template parameter...

    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    // use View channel type if floating point else use bit32f
    typedef typename channel_mapping_type<View>::type Channel;
    typedef typename boost::mpl::if_<boost::is_floating_point<Channel>, Channel, bits32f>::type ChannelFloat;
    typedef pixel<ChannelFloat, gray_layout_t> PixelGray;
    typedef image<PixelGray, false> ImageGray;
    typedef typename ImageGray::view_t ViewGray;

    OfxRectI usedMaskPixelRod; // the mask RoD (depending on the clip mask used)
    if(_clipMaskConnected)
        usedMaskPixelRod = this->_maskPixelRod;
    else
        usedMaskPixelRod = this->_srcPixelRod;

    const OfxRectI imgGradientWin = rectanglesIntersection(procWindowRoW, usedMaskPixelRod);
    const bool partialGradient =
        (imgGradientWin != procWindowRoW); // if true we can only calculate a gradient for a subpart of the procWindow
    if(_params._output == eParamOutputMotionVectors && partialGradient)
    {
        // fill the procWindow in black
        View dst =
            subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
        Pixel fillingColor = get_black(this->_srcView);
        boost::gil::fill_pixels(dst, fillingColor);
    }
    // if gradient window is empty there is nothing to do
    // so simply copy source clip
    if((imgGradientWin.y2 - imgGradientWin.y1 == 0) || (imgGradientWin.x2 - imgGradientWin.x1 == 0))
    {
        switch(_params._output)
        {
            case eParamOutputMotionVectors:
            {
                BOOST_ASSERT(partialGradient == true);
                // the output is already fill in black
                return;
            }
            case eParamOutputPushPixel:
            {
                const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);

                View src =
                    subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
                View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                         procWindowSize.y);
                copy_pixels(src, dst);
                return;
            }
        }
    }

    const OfxPointI imgGradientSize = {imgGradientWin.x2 - imgGradientWin.x1, imgGradientWin.y2 - imgGradientWin.y1};
    ImageGray xGradientImage(imgGradientSize.x, imgGradientSize.y);
    ImageGray yGradientImage(imgGradientSize.x, imgGradientSize.y);
    ViewGray xGradientView = view(xGradientImage);
    ViewGray yGradientView = view(yGradientImage);
    Point proc_mask_tl(imgGradientWin.x1 - usedMaskPixelRod.x1, imgGradientWin.y1 - usedMaskPixelRod.y1);

    /*
    TUTTLE_LOG_VAR( TUTTLE_INFO, usedMaskPixelRod );
    TUTTLE_LOG_VAR( TUTTLE_INFO, imgGradientWin );
    TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowRoW );
    TUTTLE_LOG_VAR( TUTTLE_INFO, partialGradient );
    TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowOutput );
    TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowSize );
    TUTTLE_LOG_VAR( TUTTLE_INFO, imgGradientSize );
    TUTTLE_LOG_VAR( TUTTLE_INFO, proc_mask_tl );
    */

    // compute motion vectors
    if(_clipMaskConnected)
    {
        // choose the mask channel to use
        // alpha_t if the image contains an alpha channel like rgba
        // gray_t if grayscale image
        typedef typename MaskView::value_type MaskPixel;
        typedef typename boost::mpl::if_<contains_color<MaskPixel, alpha_t>, alpha_t, gray_t>::type MaskColorChannel;
        typedef channel_view_type<MaskColorChannel, MaskView> KthChannelView;
        typename KthChannelView::type channelMaskView = KthChannelView::make(this->_maskView); // gray or alpha channel

        if(correlateMotionVectors<OfxAllocator, ViewGray, typename KthChannelView::type, Point, Scalar>(
               xGradientView, yGradientView, channelMaskView, proc_mask_tl, _params._kernelGaussianDerivative,
               _params._kernelGaussian, _params._boundary_option, this->getOfxProgress()))
            return;
    }
    else
    {
        typedef channel_view_type<alpha_t, View> KthChannelView;
        typename KthChannelView::type channelMaskView = KthChannelView::make(this->_srcView); // alpha channel

        if(correlateMotionVectors<OfxAllocator, ViewGray, typename KthChannelView::type, Point, Scalar>(
               xGradientView, yGradientView, channelMaskView, proc_mask_tl, _params._kernelGaussianDerivative,
               _params._kernelGaussian, _params._boundary_option, this->getOfxProgress()))
            return;
    }

    if(modifyVectors(xGradientView, yGradientView, this->_params._angle, _params._intensity, this->getOfxProgress()))
        return;

    // apply motion vectors
    switch(_params._output)
    {
        case eParamOutputMotionVectors:
        {
            const OfxRectI gradientRodInSrc = translateRegion(imgGradientWin, this->_srcPixelRod);
            // subdst is the part of dst for which we have motion vectors information
            View subdst = subimage_view(this->_dstView, gradientRodInSrc.x1, gradientRodInSrc.y1, imgGradientSize.x,
                                        imgGradientSize.y);
            copy_and_convert_pixels(xGradientView, kth_channel_view<0>(subdst)); // put x vectors
            if(this->progressForward(procWindowSize.y * 0.5))
                return;
            copy_and_convert_pixels(yGradientView, kth_channel_view<1>(subdst)); // put y vectors
            if(this->progressForward(procWindowSize.y * 0.5))
                return;
            return;
        }
        case eParamOutputPushPixel:
        {
            switch(_params._interpolation)
            {
                /// @todo add all interpolation methods
                case eParamInterpolationNearest:
                    motionvectors_resample_pixels<sampler::nearest_neighbor_sampler>(
                        this->_srcView, this->_srcPixelRod, xGradientView, yGradientView, imgGradientWin, this->_dstView,
                        this->_dstPixelRod, procWindowRoW, sampler::eParamFilterOutBlack, this->getOfxProgress());
                    return;
                case eParamInterpolationBilinear:
                    motionvectors_resample_pixels<sampler::bilinear_sampler>(
                        this->_srcView, this->_srcPixelRod, xGradientView, yGradientView, imgGradientWin, this->_dstView,
                        this->_dstPixelRod, procWindowRoW, sampler::eParamFilterOutBlack, this->getOfxProgress());
                    return;
            }
            TUTTLE_LOG_ERROR("Interpolation method not recognize.");
            return;
        }
    }
}
}
}
}
