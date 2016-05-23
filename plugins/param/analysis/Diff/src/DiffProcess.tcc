#include "DiffPlugin.hpp"

#include <climits>

#include <tuttle/plugin/numeric/rectOp.hpp>
#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>

namespace tuttle
{
namespace plugin
{
namespace quality
{

template <class View>
DiffProcess<View>::DiffProcess(DiffPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void DiffProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);

    _params = _plugin.getProcessParams();

    // sources view
    // clip A
    _srcA.reset(_plugin._clipSrcA->fetchImage(args.time));
    if(!_srcA.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    if(_srcA->getRowDistanceBytes() == 0)
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());

    if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
    {
        // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
        _srcPixelRodA = _plugin._clipSrcA->getPixelRod(args.time, args.renderScale);
    }
    else
    {
        _srcPixelRodA = _srcA->getRegionOfDefinition();
    }
    this->_srcViewA = this->getView(_srcA.get(), _srcPixelRodA);

    // clip B
    _srcB.reset(_plugin._clipSrcB->fetchImage(args.time));
    if(!_srcB.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    if(_srcB->getRowDistanceBytes() == 0)
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());

    if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
    {
        // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
        _srcPixelRodB = _plugin._clipSrcB->getPixelRod(args.time, args.renderScale);
    }
    else
    {
        _srcPixelRodB = _srcB->getRegionOfDefinition();
    }
    this->_srcViewB = this->getView(_srcB.get(), _srcPixelRodB);

    // Make sure bit depths are the same
    if(_srcA->getPixelDepth() != this->_dst->getPixelDepth() || _srcB->getPixelDepth() != this->_dst->getPixelDepth())
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("Diff: bit depth mismatch"));
    }
    if(_srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
       _srcB->getPixelComponents() != this->_dst->getPixelComponents())
    {
        BOOST_THROW_EXCEPTION(exception::InputMismatch() << exception::user("Diff: components mismatch"));
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void DiffProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View srcViewA = subimage_view(this->_srcViewA, procWindowRoW.x1 - _srcPixelRodA.x1, procWindowRoW.y1 - _srcPixelRodA.y1,
                                  procWindowSize.x, procWindowSize.y);
    View srcViewB = subimage_view(this->_srcViewB, procWindowRoW.x1 - _srcPixelRodB.x1, procWindowRoW.y1 - _srcPixelRodB.y1,
                                  procWindowSize.x, procWindowSize.y);
    View dstView = subimage_view(this->_dstView, procWindowRoW.x1 - this->_dstPixelRod.x1,
                                 procWindowRoW.y1 - this->_dstPixelRod.y1, procWindowSize.x, procWindowSize.y);

    rgba32f_pixel_t paramRgbaValue(0, 0, 0, 0);

    switch(_params.measureFunction)
    {
        case eMeasureFunctionMSE:
            color_convert(mse(srcViewA, srcViewB, dstView), paramRgbaValue);
            break;
        case eMeasureFunctionPSNR:
            color_convert(psnr(srcViewA, srcViewB, dstView), paramRgbaValue);
            break;
        case eMeasureFunctionSSIM:
            break;
    }

    _plugin._qualityMesure->setValueAtTime(this->_renderArgs.time, get_color(paramRgbaValue, red_t()),
                                           get_color(paramRgbaValue, green_t()), get_color(paramRgbaValue, blue_t()),
                                           get_color(paramRgbaValue, alpha_t()));
}

template <class View>
template <class SView>
boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
DiffProcess<View>::psnr(const SView& v1, const SView& v2, const SView& dst)
{
    typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
        Pixel32F;
    typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
    typedef typename boost::gil::channel_type<typename SView::value_type>::type SValueType;

    size_t d = (size_t)(std::pow(2.0, sizeof(SValueType) * 8.0)) - 1;
    size_t d2 = d * d;
    Pixel32F psnr = mse(v1, v2, dst, true);
    for(int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i)
    {
        float p = (float)d2 - ((float)d2 / psnr[i]);
        if(p > std::numeric_limits<float>::epsilon())
            psnr[i] = Value32F(10.0 * std::log10(p));
        else
            psnr[i] = 0;
    }
    return psnr;
}

template <class View>
template <class SView>
boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
DiffProcess<View>::mse(const SView& v1, const SView& v2, const SView& dst, bool outputIsPsnr)
{
    using namespace terry;
    typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
        Pixel32F;
    typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
    typedef typename boost::gil::channel_type<typename SView::value_type>::type SValueType;

    Pixel32F veqm = get_black<Pixel32F>();
    int w = v1.width();
    int h = v1.height();

    size_t d = (size_t)(std::pow(2.0, sizeof(SValueType) * 8.0)) - 1;
    size_t d2 = d * d;

    for(typename SView::y_coord_t y = 0; y < h; ++y)
    {
        typename SView::x_iterator itA = v1.row_begin(y);
        typename SView::x_iterator itB = v2.row_begin(y);
        typename SView::x_iterator itD = dst.row_begin(y);

        for(typename SView::x_coord_t x = 0; x < w; ++x)
        {
            for(int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i)
            {
                Value32F diff = (Value32F)std::abs(double((*itA)[i] - (*itB)[i]));

                if(outputIsPsnr)
                {
                    float p = (float)d2 / diff;
                    if(p > std::numeric_limits<float>::epsilon())
                        (*itD)[i] = Value32F(10.0 * std::log10(p));
                    else
                        (*itD)[i] = 0;
                }
                else
                {
                    (*itD)[i] = diff;
                }
                veqm[i] += diff * diff;
            }
            ++itA;
            ++itB;
            ++itD;
        }
        this->progressUpdate((float)(y + 1) / h);
    }

    for(int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i)
    {
        veqm[i] /= (w * h);
    }
    return veqm;
}
}
}
}
