#include "CropPlugin.hpp"

#include <tuttle/plugin/numeric/rectOp.hpp>
#include <tuttle/plugin/ofxToGil/rect.hpp>

#include <terry/math/Rect.hpp>
#include <terry/draw/fill.hpp>
#include <terry/basic_colors.hpp>
#include <terry/globals.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace crop
{

template <class View>
CropProcess<View>::CropProcess(CropPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromBottomToTop)
    , _plugin(instance)
{
}

template <class View>
void CropProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams<Pixel>(args.time, args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void CropProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    // Currently, it's not possible to place the crop depending on the RoW and not on image source RoD.

    const OfxRectI procWindowOutput = translateRegion(procWindowRoW, this->_dstPixelRod);

    terry::draw::fill_pixels(this->_dstView, ofxToGil(procWindowOutput), _params._color);

    // proc region of source image to copy
    const OfxRectI procCropRoW =
        rectanglesIntersection(rectanglesIntersection(_params._cropRegion, procWindowRoW), this->_srcPixelRod);

    const OfxRectI procCropOutput = translateRegion(procCropRoW, this->_dstPixelRod);
    const OfxRectI procCropSrc = translateRegion(procCropRoW, this->_srcPixelRod);
    const OfxPointI procCropSize = {procCropRoW.x2 - procCropRoW.x1, procCropRoW.y2 - procCropRoW.y1};

    View src = subimage_view(this->_srcView, procCropSrc.x1, procCropSrc.y1, procCropSize.x, procCropSize.y);
    View dst = subimage_view(this->_dstView, procCropOutput.x1, procCropOutput.y1, procCropSize.x, procCropSize.y);

    copy_pixels(src, dst);
}
}
}
}
