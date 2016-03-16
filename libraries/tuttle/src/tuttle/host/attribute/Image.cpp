#include "Image.hpp"

#if(TUTTLE_PNG_EXPORT_BETWEEN_NODES)
#define int_p_NULL (int*) NULL
// Should be included first to avoid setjmp.h include troubles
#include <boost/gil/extension/io/png_io.hpp>
#endif

#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/common/utils/global.hpp>

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

Image::Image(ClipImage& clip, const OfxTime time, const OfxRectD& bounds, const EImageOrientation orientation,
             const int rowDistanceBytes)
    : ofx::imageEffect::OfxhImage(clip, time) ///< this ctor will set basic props on the image
    , _memorySize(0)
    , _pixelBytes(clip.getPixelMemorySize())
    , _rowAbsDistanceBytes(0)
    , _orientation(orientation)
    , _fullname(clip.getFullName())
{
    // Set rod in canonical & pixel coord.
    const double par = clip.getPixelAspectRatio();
    _bounds.x1 = std::floor(bounds.x1 / par);
    _bounds.x2 = std::ceil(bounds.x2 / par);
    _bounds.y1 = std::floor(bounds.y1);
    _bounds.y2 = std::ceil(bounds.y2);

    const OfxPointI dimensions = {_bounds.x2 - _bounds.x1, _bounds.y2 - _bounds.y1};

    // make some memory according to the bit depth
    const std::size_t automaticRowSize = dimensions.x * _pixelBytes;
    _memorySize = automaticRowSize * dimensions.y;

    // render scale x and y of 1.0
    setDoubleProperty(kOfxImageEffectPropRenderScale, 1.0, 0);
    setDoubleProperty(kOfxImageEffectPropRenderScale, 1.0, 1);

    // bounds and rod
    setIntProperty(kOfxImagePropBounds, _bounds.x1, 0);
    setIntProperty(kOfxImagePropBounds, _bounds.y1, 1);
    setIntProperty(kOfxImagePropBounds, _bounds.x2, 2);
    setIntProperty(kOfxImagePropBounds, _bounds.y2, 3);

    /// @todo the same for bounds and rod, no tiles for the moment !
    setIntProperty(kOfxImagePropRegionOfDefinition, _bounds.x1, 0);
    setIntProperty(kOfxImagePropRegionOfDefinition, _bounds.y1, 1);
    setIntProperty(kOfxImagePropRegionOfDefinition, _bounds.x2, 2);
    setIntProperty(kOfxImagePropRegionOfDefinition, _bounds.y2, 3);

    // row bytes
    _rowAbsDistanceBytes = rowDistanceBytes != 0 ? rowDistanceBytes : automaticRowSize;
    setIntProperty(kOfxImagePropRowBytes, getOrientedRowDistanceBytes(eImageOrientationFromBottomToTop));
}

Image::~Image()
{
    // TUTTLE_LOG_VAR( TUTTLE_TRACE, getFullName() );
}

boost::uint8_t* Image::getPixelData()
{
    return reinterpret_cast<boost::uint8_t*>(_data->data());
}

void* Image::getVoidPixelData()
{
    return reinterpret_cast<void*>(_data->data());
}

char* Image::getCharPixelData()
{
    return reinterpret_cast<char*>(_data->data());
}

boost::uint8_t* Image::getOrientedPixelData(const EImageOrientation orientation)
{
    if(_orientation == orientation)
    {
        return getPixelData();
    }
    else
    {
        const std::ssize_t distance = getRowAbsDistanceBytes() * (_bounds.y2 - _bounds.y1 - 1);
        return reinterpret_cast<boost::uint8_t*>(getPixelData() + distance);
    }
}

boost::uint8_t* Image::pixel(const int x, const int y)
{
    const OfxRectI bounds = getBounds();

    if((x >= bounds.x1) && (x < bounds.x2) && (y >= bounds.y1) && (y < bounds.y2))
    {
        const int yOffset = (_orientation == eImageOrientationFromTopToBottom) ? (y - bounds.y1) : (y - bounds.y2);
        const int offset = yOffset * getRowAbsDistanceBytes() + (x - bounds.x1) * _pixelBytes;
        return reinterpret_cast<boost::uint8_t*>(getPixelData() + offset);
    }
    return NULL;
}

template <class D_VIEW, class S_VIEW>
void Image::copy(D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner, const OfxPointI& srcCorner, const OfxPointI& count)
{
    using namespace boost::gil;
    if(src.width() >= (count.x - srcCorner.x) && src.height() >= (count.y - srcCorner.y) &&
       dst.width() >= (count.x - dstCorner.x) && dst.height() >= (count.y - dstCorner.y))
    {
        S_VIEW subSrc = subimage_view(src, srcCorner.x, srcCorner.y, count.x, count.y);
        D_VIEW subDst = subimage_view(dst, dstCorner.x, dstCorner.y, count.x, count.y);
        copy_and_convert_pixels(subSrc, subDst);
    }
}

#if(TUTTLE_PNG_EXPORT_BETWEEN_NODES)
void Image::debugSaveAsPng(const std::string& filename)
{
    using namespace boost::gil;
    switch(getComponentsType())
    {
        case ofx::imageEffect::ePixelComponentRGB:
            switch(getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    rgb8_view_t view = getGilView<rgb8_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    rgb16_view_t view = getGilView<rgb16_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    rgb32f_view_t view = getGilView<rgb32f_view_t>();
                    png_write_view(filename, color_converted_view<rgb8_pixel_t>(view));
                    break;
                }
                default:
                    break;
            }
            break;
        case ofx::imageEffect::ePixelComponentRGBA:
            switch(getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    rgba8_view_t view = getGilView<rgba8_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    rgba16_view_t view = getGilView<rgba16_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    rgba32f_view_t view = getGilView<rgba32f_view_t>();
                    png_write_view(filename, color_converted_view<rgba8_pixel_t>(view));
                    break;
                }
                default:
                    break;
            }
            break;
        case ofx::imageEffect::ePixelComponentAlpha:
            switch(getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    gray8_view_t view = getGilView<gray8_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    gray16_view_t view = getGilView<gray16_view_t>();
                    png_write_view(filename, view);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    gray32f_view_t view = getGilView<gray32f_view_t>();
                    png_write_view(filename, color_converted_view<rgb8_pixel_t>(view));
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            TUTTLE_LOG_WARNING("Cannot write png debug image for file '" + filename + "': unsupported component type.");
            break;
    }
}
#endif

/// Copy from gil image view to Image
template <class S_VIEW>
void Image::copy(Image* dst, S_VIEW& src, const OfxPointI& dstCorner, const OfxPointI& srcCorner, const OfxPointI& count)
{
    using namespace boost::gil;
    // Create destination
    switch(dst->getComponentsType())
    {
        case ofx::imageEffect::ePixelComponentRGBA:
            switch(dst->getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    rgba8_view_t dView = dst->getGilView<rgba8_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    rgba16_view_t dView = dst->getGilView<rgba16_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    rgba32f_view_t dView = dst->getGilView<rgba32f_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                default:
                    break;
            }
            break;
        case ofx::imageEffect::ePixelComponentAlpha:
            switch(dst->getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    gray8_view_t dView = dst->getGilView<gray8_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    gray16_view_t dView = dst->getGilView<gray16_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    gray32f_view_t dView = dst->getGilView<gray32f_view_t>();
                    Image::copy(dView, src, dstCorner, srcCorner, count);
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

/// Copy from Image to Image
void Image::copy(Image* dst, Image* src, const OfxPointI& dstCorner, const OfxPointI& srcCorner, const OfxPointI& count)
{
    using namespace boost::gil;
    switch(src->getComponentsType())
    {
        case ofx::imageEffect::ePixelComponentRGBA:
            switch(src->getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    rgba8_view_t sView = src->getGilView<rgba8_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    rgba16_view_t sView = src->getGilView<rgba16_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    rgba32f_view_t sView = src->getGilView<rgba32f_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                default:
                    break;
            }
            break;
        case ofx::imageEffect::ePixelComponentAlpha:
            switch(src->getBitDepth())
            {
                case ofx::imageEffect::eBitDepthUByte:
                {
                    gray8_view_t sView = src->getGilView<gray8_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthUShort:
                {
                    gray16_view_t sView = src->getGilView<gray16_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                case ofx::imageEffect::eBitDepthFloat:
                {
                    gray32f_view_t sView = src->getGilView<gray32f_view_t>();
                    Image::copy(dst, sView, dstCorner, srcCorner, count);
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }
}
}
}
}
