#include "FadeAlgorithm.hpp"

#include <terry/merge/ViewsMerging.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/generator/constant.hpp>
#include <terry/typedefs.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace fade
{

template <typename Pixel>
struct FunctorFade : public terry::merge_functor<Pixel, terry::merge_per_channel>
{
    const double _t;

    FunctorFade(const double t)
        : _t(t)
    {
    }

    template <typename Channel>
    inline void operator()(const Channel& A, const Channel& B, Channel& dst)
    {
        // (1-t)*A + t*B
        dst = (Channel)((1.0 - _t) * A + _t * B);
    }
};

template <typename Pixel>
struct FunctorFadeToColor
{
    const Pixel _color;
    const double _t;
    const double _invt; // 1.0 - _t

    FunctorFadeToColor(const Pixel& color, const double t)
        : _color(color)
        , _t(t)
        , _invt(1.0 - t)
    {
    }

    GIL_FORCEINLINE
    Pixel operator()(const Pixel& a) const
    {
        using namespace terry::numeric;
        // (1-t)*src + t*color
        return pixel_plus_t<Pixel, Pixel, Pixel>()(pixel_multiplies_scalar_t<Pixel, double>()(a, _invt),
                                                   pixel_multiplies_scalar_t<Pixel, double>()(_color, _t));
    }
};

template <class View>
FadeProcess<View>::FadeProcess(FadePlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void FadeProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);

    // sources view
    // clip A
    if(_plugin._clipSrcFrom->isConnected())
    {
        _srcA.reset(_plugin._clipSrcFrom->fetchImage(args.time));
        if(!_srcA.get())
            BOOST_THROW_EXCEPTION(exception::ImageNotReady());
        if(_srcA->getRowDistanceBytes() == 0)
            BOOST_THROW_EXCEPTION(exception::WrongRowBytes());

        if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
        {
            // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
            _srcPixelRodA = _plugin._clipSrcFrom->getPixelRod(args.time, args.renderScale);
        }
        else
        {
            _srcPixelRodA = _srcA->getRegionOfDefinition();
        }
        this->_srcViewA = this->getView(_srcA.get(), _srcPixelRodA);
    }
    // clip B
    if(_plugin._clipSrcTo->isConnected())
    {
        _srcB.reset(_plugin._clipSrcTo->fetchImage(args.time));
        if(!_srcB.get())
            BOOST_THROW_EXCEPTION(exception::ImageNotReady());
        if(_srcB->getRowDistanceBytes() == 0)
            BOOST_THROW_EXCEPTION(exception::WrongRowBytes());

        if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
        {
            // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
            _srcPixelRodB = _plugin._clipSrcTo->getPixelRod(args.time, args.renderScale);
        }
        else
        {
            _srcPixelRodB = _srcB->getRegionOfDefinition();
        }
        this->_srcViewB = this->getView(_srcB.get(), _srcPixelRodB);
    }
    if(_plugin._clipSrcFrom->isConnected() && _plugin._clipSrcTo->isConnected())
    {
        // Make sure bit depths are the same
        if(_srcA->getPixelDepth() != this->_dst->getPixelDepth() || _srcB->getPixelDepth() != this->_dst->getPixelDepth() ||
           _srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
           _srcB->getPixelComponents() != this->_dst->getPixelComponents())
        {
            BOOST_THROW_EXCEPTION(exception::BitDepthMismatch());
        }
    }

    _params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void FadeProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;

    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    const OfxRectI procWindowOutput = translateRegion(procWindowRoW, this->_dstPixelRod);
    const OfxRectI srcRodA = _srcPixelRodA; // translateRegion( _srcPixelRodA, _params._offsetA );
    const OfxRectI srcRodB = _srcPixelRodB; // translateRegion( _srcPixelRodB, _params._offsetB );

    const bool fromConnected = _plugin._clipSrcFrom->isConnected();
    const bool toConnected = _plugin._clipSrcTo->isConnected();

    if(fromConnected && toConnected)
    {

        const OfxRectI intersect = rectanglesIntersection(srcRodA, srcRodB);
        const OfxRectI procIntersect = rectanglesIntersection(procWindowRoW, intersect);
        const OfxPointI procIntersectSize = {procIntersect.x2 - procIntersect.x1, procIntersect.y2 - procIntersect.y1};

        /// @todo tuttle: fill only the good regions
        switch(_params._rod)
        {
            case eParamRodIntersect:
            {
                // nothing to do in this case
                // this is the standard case
                // we merge the intersection of the 2 images
                break;
            }
            case eParamRodUnion:
            {
                /// @todo tuttle:
                ///  * add color choice
                ///  * fill A and B
                ///  * fill only the good regions
                {
                    // fill color
                    Pixel pixelZero;
                    pixel_zeros_t<Pixel>()(pixelZero);
                    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                             procWindowSize.y);
                    fill_pixels(dst, pixelZero);
                }
                {
                    // fill with A
                    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, srcRodA);

                    View src = subimage_view(this->_srcViewA, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x,
                                             procWindowSize.y);
                    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                             procWindowSize.y);
                    /// @todo tuttle: fill only the good regions
                    copy_pixels(src, dst);
                }
                {
                    // fill with B
                    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, srcRodB);

                    View src = subimage_view(this->_srcViewB, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x,
                                             procWindowSize.y);
                    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                             procWindowSize.y);
                    /// @todo tuttle: fill only the good regions
                    copy_pixels(src, dst);
                }
                break;
            }
            case eParamRodA:
            {
                // fill with A
                const OfxRectI procWindowSrc = translateRegion(procWindowRoW, srcRodA);

                View src =
                    subimage_view(this->_srcViewA, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
                View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                         procWindowSize.y);
                /// @todo tuttle: fill only the good regions
                copy_pixels(src, dst);
                break;
            }
            case eParamRodB:
            {
                // fill with B
                const OfxRectI procWindowSrc = translateRegion(procWindowRoW, srcRodB);

                View src =
                    subimage_view(this->_srcViewB, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
                View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x,
                                         procWindowSize.y);
                /// @todo tuttle: fill only the good regions
                copy_pixels(src, dst);
                break;
            }
        }

        View srcViewA_inter = subimage_view(this->_srcViewA, procIntersect.x1 - srcRodA.x1, procIntersect.y1 - srcRodA.y1,
                                            procIntersectSize.x, procIntersectSize.y);
        View srcViewB_inter = subimage_view(this->_srcViewB, procIntersect.x1 - srcRodB.x1, procIntersect.y1 - srcRodB.y1,
                                            procIntersectSize.x, procIntersectSize.y);

        View dstView_inter =
            subimage_view(this->_dstView, procIntersect.x1 - this->_dstPixelRod.x1, procIntersect.y1 - this->_dstPixelRod.y1,
                          procIntersectSize.x, procIntersectSize.y);

        merge_views(srcViewA_inter, srcViewB_inter, dstView_inter, FunctorFade<Pixel>(_params._transition));
    }
    else if(fromConnected || toConnected)
    {
        // Only one input clip connected: fade to color
        View connectedSrc = fromConnected ? this->_srcViewA : this->_srcViewB;

        const OfxRectI procWindowSrc = translateRegion(procWindowRoW, fromConnected ? srcRodA : srcRodB);

        View src = subimage_view(connectedSrc, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
        View dst =
            subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

        // Fade to color
        Pixel c;
        color_convert(_params._color, c);

        terry::algorithm::transform_pixels_progress(src, dst, FunctorFadeToColor<Pixel>(c, _params._transition),
                                                    this->getOfxProgress());
    }
    else
    {
        // No input clip, set a constant color
        View dst =
            subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

        Pixel c;
        color_convert(_params._color, c);

        terry::algorithm::transform_pixels_progress(dst, pixel_assigns_color_t<Pixel>(c), this->getOfxProgress());
    }
}
}
}
}
