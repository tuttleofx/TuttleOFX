#include "InputBufferWrapper.hpp"
#include "Core.hpp"
#include "exceptions.hpp"
#include "memory/LinkData.hpp"

#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>

#include <map>

namespace tuttle
{
namespace host
{

using namespace boost::assign;

void InputBufferWrapper::setMode(const EMode mode)
{
    static std::map<EMode, const char*> toString =
        map_list_of(eModeBuffer, "bufferPointer")(eModeCallback, "callbackPointer");

    getNode().getParam("mode").setValue(toString.find(mode)->second);
}

void InputBufferWrapper::setBuffer(void* rawBuffer)
{
    getNode()
        .getParam("bufferPointer")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(rawBuffer)));
}

void InputBufferWrapper::set2DArrayBuffer(void* rawBuffer, const int width, const int height)
{
    setBuffer(rawBuffer);
    setSize(width, height);
    setComponents(ePixelComponentAlpha);
}

void InputBufferWrapper::set3DArrayBuffer(void* rawBuffer, const int width, const int height, const int nbComponents)
{
    TUTTLE_LOG_INFOS;
    TUTTLE_LOG_INFO("[Inpput buffer wrapper] width = " << width << ", height = " << height
                                                       << ", components = " << nbComponents);
    setBuffer(rawBuffer);
    setSize(width, height);
    switch(nbComponents)
    {
        case 1:
            setComponents(ePixelComponentAlpha);
            break;
        case 3:
            setComponents(ePixelComponentRGB);
            break;
        case 4:
            setComponents(ePixelComponentRGBA);
            break;
        default:
            BOOST_THROW_EXCEPTION(exception::Value() << exception::dev() + "Unrecognized component size: " + nbComponents);
            break;
    }
}

void InputBufferWrapper::setSize(const int width, const int height)
{
    getNode().getParam("size").setValue(width, height);
}

void InputBufferWrapper::setComponents(const EPixelComponent components)
{
    static std::map<EPixelComponent, const char*> toString =
        map_list_of(ePixelComponentRGBA, "RGBA")(ePixelComponentRGB, "RGB")(ePixelComponentAlpha, "Gray");

    getNode().getParam("components").setValue(toString.find(components)->second);
}

void InputBufferWrapper::setBitDepth(const EBitDepth bitDepth)
{
    static std::map<EBitDepth, const char*> toString =
        map_list_of(eBitDepthFloat, "Float")(eBitDepthUShort, "UShort")(eBitDepthUByte, "UByte");

    getNode().getParam("bitDepth").setValue(toString.find(bitDepth)->second);
}

void InputBufferWrapper::setRowDistanceSize(const int rowDistanceBytes)
{
    getNode().getParam("rowBytesSize").setValue(rowDistanceBytes);
}

void InputBufferWrapper::setOrientation(const EImageOrientation orientation)
{
    static std::map<EImageOrientation, const char*> toString =
        map_list_of(eImageOrientationFromBottomToTop, "bottomToTop")(eImageOrientationFromTopToBottom, "topToBottom");

    getNode().getParam("orientation").setValue(toString.find(orientation)->second);
}

void InputBufferWrapper::setRawImageBuffer(void* rawBuffer, const int width, const int height,
                                           const EPixelComponent components, const EBitDepth bitDepth,
                                           const int rowDistanceBytes, const EImageOrientation orientation)
{
    setMode(eModeBuffer);
    setBuffer(rawBuffer);
    setSize(width, height);
    setComponents(components);
    setBitDepth(bitDepth);
    setRowDistanceSize(rowDistanceBytes);
    setOrientation(orientation);

    //	_outputClip.setComponents( components );
    //	_outputClip.setBitDepth( bitDepth );
    //	_imageCache.reset( new attribute::Image(
    //			_outputClip,
    //			0, // no time information
    //			rod,
    //			orientation,
    //			rowDistanceBytes
    //		) );
    //	_imageCache->setPoolData( new memory::LinkData( rawBuffer ) );
}

// void InputBufferWrapper::process( graph::ProcessVertexAtTimeData& vData )
//{
//	core().getMemoryCache().put( _outputClip.getClipIdentifier(), vData._time, _imageCache );
//	if( vData._outDegree > 0 )
//	{
//		_imageCache->addReference( ofx::imageEffect::OfxhImage::eReferenceOwnerHost, vData._outDegree ); // add a
// reference on this getNode() for each future usages
//	}
//}

void InputBufferWrapper::setCallback(CallbackInputImagePtr callback, CustomDataPtr customData,
                                     CallbackDestroyCustomDataPtr destroyCustomData)
{
    getNode()
        .getParam("callbackPointer")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(callback)));
    getNode()
        .getParam("customData")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(customData)));
    getNode()
        .getParam("callbackDestroyCustomData")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(destroyCustomData)));
}
}
}
