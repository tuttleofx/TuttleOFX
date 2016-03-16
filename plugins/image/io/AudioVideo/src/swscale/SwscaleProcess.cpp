#include "SwscaleProcess.hpp"

#include <AvTranscoder/data/decoded/VideoFrame.hpp>

namespace tuttle
{
namespace plugin
{
namespace swscale
{

SwscaleProcess::SwscaleProcess(SwscalePlugin& effect)
    : ImageFilterProcessor(effect, eImageOrientationFromTopToBottom)
    , _plugin(effect)
    , _params()
    , _transform()
{
    this->setNoMultiThreading();
}

void SwscaleProcess::setup(const OFX::RenderArguments& args)
{
    ImageFilterProcessor::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

PixelFormat ofxPixelComponentToSwsPixelFormat(const OFX::EPixelComponent component, const OFX::EBitDepth bitDepth)
{
    switch(component)
    {
        case OFX::ePixelComponentRGBA:
            switch(bitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("Dpx: Unkown bit depth"));
                    break;
                case OFX::eBitDepthUByte:
                    return PIX_FMT_RGBA;
                case OFX::eBitDepthUShort:
                    return PIX_FMT_NONE;
                case OFX::eBitDepthFloat:
                    return PIX_FMT_NONE;
            }
            break;
        case OFX::ePixelComponentRGB:
            switch(bitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("Dpx: Unkown bit depth"));
                    break;
                case OFX::eBitDepthUByte:
                    return PIX_FMT_RGB24;
                case OFX::eBitDepthUShort:
                    return PIX_FMT_RGB48;
                case OFX::eBitDepthFloat:
                    return PIX_FMT_NONE;
            }
            break;
        case OFX::ePixelComponentAlpha:
            switch(bitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("Dpx: Unkown bit depth"));
                    break;
                case OFX::eBitDepthUByte:
                    return PIX_FMT_GRAY8;
                case OFX::eBitDepthUShort:
                    return PIX_FMT_GRAY16;
                case OFX::eBitDepthFloat:
                    return PIX_FMT_NONE;
            }
            break;
        case OFX::ePixelComponentNone:
        case OFX::ePixelComponentCustom:
            return PIX_FMT_NONE;
    }
    BOOST_ASSERT(false);
    return PIX_FMT_NONE;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
void SwscaleProcess::multiThreadProcessImages(const OfxRectI& procWindow)
{
    OFX::EPixelComponent component = this->_src->getPixelComponents();
    OFX::EBitDepth bitDepth = this->_src->getPixelDepth();
    PixelFormat pixFmt = ofxPixelComponentToSwsPixelFormat(component, bitDepth);

    if(pixFmt == PIX_FMT_NONE)
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                              << exception::user("SwScale: unsupported bit depth / channel input."));
    }

    // set filtering mode
    // @todo: sws set filtermode( _params._sws_filter )

    // for tile support... (currently disabled)
    //	void* srcPtr = this->_src->getPixelAddress( procWindow.x1, procWindow.y1 );
    //	void* dstPtr = this->_dst->getPixelAddress( procWindow.x1, procWindow.y1 );

    // for simple buffer access
    uint8_t* srcPtr = (uint8_t*)this->_src->getPixelData();
    uint8_t* dstPtr = (uint8_t*)this->_dst->getPixelData();
    if(!srcPtr || !dstPtr)
    {
        BOOST_THROW_EXCEPTION(exception::Failed() << exception::user("swscale: Pixel data pointer invalid."));
    }

    // "this->_src->getRowDistanceBytes()" is not the same than "this->_src->getBoundsSize().x * pixelBytes"
    // The buffer could contain a padding between lines.

    avtranscoder::VideoFrameDesc srcDesc(_src->getBoundsSize().x, _src->getBoundsSize().y, pixFmt);
    avtranscoder::VideoFrame src(srcDesc);
    src.getData()[0] = srcPtr;

    avtranscoder::VideoFrameDesc dstDesc(_dst->getBoundsSize().x, _dst->getBoundsSize().y, pixFmt);
    avtranscoder::VideoFrame dst(dstDesc);
    dst.getData()[0] = dstPtr;

    _transform.convert(src, dst);
}
}
}
}
