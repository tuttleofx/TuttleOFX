#ifndef _TUTTLE_HOST_INPUTBUFFERNODE_HPP_
#define _TUTTLE_HOST_INPUTBUFFERNODE_HPP_

#include "INode.hpp"
#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

namespace tuttle
{
namespace host
{

class InputBufferWrapper
{
public:
    enum EMode
    {
        eModeBuffer,
        eModeCallback
    };
    enum EPixelComponent
    {
        ePixelComponentRGBA,
        ePixelComponentRGB,
        ePixelComponentAlpha
    };
    enum EBitDepth
    {
        eBitDepthUByte,
        eBitDepthUShort,
        eBitDepthFloat
    };
    enum EImageOrientation
    {
        eImageOrientationFromTopToBottom,
        eImageOrientationFromBottomToTop
    };
    enum EField
    {
        eFieldBoth,  //< @brief fielded image with both fields present
        eFieldLower, //< @brief only the spatially lower field is present
        eFieldUpper  //< @brief only the spatially upper field is present
    };

    typedef void* CustomDataPtr;
    typedef void (*CallbackInputImagePtr)(OfxTime time, CustomDataPtr outputCustomData, void** rawdata, int* width,
                                          int* height, int* rowSizeBytes);
    typedef void (*CallbackDestroyCustomDataPtr)(CustomDataPtr outputCustomData);

private:
    INode* _node;

public:
    InputBufferWrapper(INode& node)
        : _node(&node)
    {
    }
    InputBufferWrapper()
        : _node(NULL)
    {
    }
    ~InputBufferWrapper() {}

    INode& getNode() { return *_node; }

    void setMode(const EMode mode);
    void setBuffer(void* rawBuffer);

private:
    void set2DArrayBuffer(void* rawBuffer, const int width, const int height);
    void set3DArrayBuffer(void* rawBuffer, const int width, const int height, const int nbComponents);

public:
    void set2DArrayBuffer(unsigned char* rawBuffer, int height, int width)
    {
        set2DArrayBuffer((void*)rawBuffer, width, height);
        setBitDepth(eBitDepthUByte);
    }
    void set3DArrayBuffer(unsigned char* rawBuffer, int height, int width, int nbComponents)
    {
        set3DArrayBuffer((void*)rawBuffer, width, height, nbComponents);
        setBitDepth(eBitDepthUByte);
    }

    void set2DArrayBuffer(unsigned short* rawBuffer, int height, int width)
    {
        set2DArrayBuffer((void*)rawBuffer, width, height);
        setBitDepth(eBitDepthUShort);
    }
    void set3DArrayBuffer(unsigned short* rawBuffer, int height, int width, int nbComponents)
    {
        set3DArrayBuffer((void*)rawBuffer, width, height, nbComponents);
        setBitDepth(eBitDepthUShort);
    }

    void set2DArrayBuffer(float* rawBuffer, int height, int width)
    {
        set2DArrayBuffer((void*)rawBuffer, width, height);
        setBitDepth(eBitDepthUShort);
    }
    void set3DArrayBuffer(float* rawBuffer, int height, int width, int nbComponents)
    {
        set3DArrayBuffer((void*)rawBuffer, width, height, nbComponents);
        setBitDepth(eBitDepthUShort);
    }

    void setSize(const int width, const int height);
    void setComponents(const EPixelComponent components);
    void setBitDepth(const EBitDepth bitDepth);
    void setRowDistanceSize(const int rowDistanceBytes);
    void setOrientation(const EImageOrientation orientation);

    void setRawImageBuffer(void* rawBuffer, const int width, const int height, const EPixelComponent components,
                           const EBitDepth bitDepth, const int rowDistanceBytes = 0,
                           const EImageOrientation orientation = eImageOrientationFromBottomToTop);

    void setRawImageBuffer(unsigned char* rawBuffer, const int width, const int height, const EPixelComponent components,
                           const int rowDistanceBytes = 0,
                           const EImageOrientation orientation = eImageOrientationFromBottomToTop)
    {
        setRawImageBuffer(reinterpret_cast<void*>(rawBuffer), width, height, components, eBitDepthUByte, rowDistanceBytes,
                          orientation);
    }

    void setRawImageBuffer(unsigned short* rawBuffer, const int width, const int height, const EPixelComponent components,
                           const int rowDistanceBytes = 0,
                           const EImageOrientation orientation = eImageOrientationFromBottomToTop)
    {
        setRawImageBuffer(reinterpret_cast<void*>(rawBuffer), width, height, components, eBitDepthUShort, rowDistanceBytes,
                          orientation);
    }

    void setRawImageBuffer(float* rawBuffer, const int width, const int height, const EPixelComponent components,
                           const int rowDistanceBytes = 0,
                           const EImageOrientation orientation = eImageOrientationFromBottomToTop)
    {
        setRawImageBuffer(reinterpret_cast<void*>(rawBuffer), width, height, components, eBitDepthFloat, rowDistanceBytes,
                          orientation);
    }

    void setCallback(CallbackInputImagePtr callback, CustomDataPtr customData = NULL,
                     CallbackDestroyCustomDataPtr destroyCustomData = NULL);
};
}
}

#endif
