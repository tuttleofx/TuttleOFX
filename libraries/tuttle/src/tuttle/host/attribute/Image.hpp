#ifndef TUTTLE_HOST_CORE_IMAGE_HPP
#define TUTTLE_HOST_CORE_IMAGE_HPP

#include <tuttle/host/ofx/OfxhImage.hpp>
#include <tuttle/common/ofx/imageEffect.hpp>

#include <tuttle/host/memory/IMemoryPool.hpp>

/// @tuttle: remove include dependencies to gil
#include <boost/gil/channel_algorithm.hpp> // force include boostHack first
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <boost/cstdint.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

class ClipImage;

/**
 * make an image up
 */
class Image : public tuttle::host::ofx::imageEffect::OfxhImage
{
public:
    enum EImageOrientation
    {
        eImageOrientationFromTopToBottom, //< Use image orientation from top to bottom
        eImageOrientationFromBottomToTop  //< Use image orientation from bottom to top
    };

protected:
    std::size_t _memorySize;  ///< memory size
    std::size_t _pixelBytes;  ///< pixel memory size
    int _rowAbsDistanceBytes; ///< positive memory size for the distance between rows
    OfxRectI _bounds;
    EImageOrientation _orientation;
    std::string _fullname;
    memory::IPoolDataPtr _data; ///< where we are keeping our image data

public:
    Image(ClipImage& clip, const OfxTime time, const OfxRectD& bounds, const EImageOrientation orientation,
          const int rowDistanceBytes);
    virtual ~Image();

#ifndef SWIG
    memory::IPoolDataPtr& getPoolData() { return _data; }
    const memory::IPoolDataPtr& getPoolData() const { return _data; }

    void setPoolData(const memory::IPoolDataPtr& pData)
    {
        _data = pData;
        setPointerProperty(kOfxImagePropData,
                           getOrientedPixelData(eImageOrientationFromBottomToTop)); // OpenFX standard use BottomToTop
    }
#endif

    std::string getFullName() const { return _fullname; }

    std::size_t getMemorySize() const { return _memorySize; }
    /**
     * @brief Positive/Absolute distance rows.
     */
    int getRowAbsDistanceBytes() const { return _rowAbsDistanceBytes; }

    EImageOrientation getOrientation() const { return _orientation; }

    std::size_t getNbComponents() const { return ofx::imageEffect::numberOfComponents(getComponentsType()); }

    /**
     * @brief Get distance between rows depending on the requested orientation.
     */
    int getOrientedRowDistanceBytes(const EImageOrientation orientation)
    {
        return _rowAbsDistanceBytes * (_orientation != orientation ? -1 : 1);
    }

    boost::uint8_t* getPixelData();
    void* getVoidPixelData();
    char* getCharPixelData();
    boost::uint8_t* getOrientedPixelData(const EImageOrientation orientation);

    boost::uint8_t* pixel(const int x, const int y);

    /**
     * @todo clean this!
     * move outside from class or use copyFrom (don't specify dst)
     * use ref, change order, etc.
     */
    static void copy(Image* dst, Image* src, const OfxPointI& dstCorner, const OfxPointI& srcCorner, const OfxPointI& count);

    void getImage(boost::uint8_t*& outData, int& outWidth, int& outHeight, int& outRowSizeBytes,
                  ofx::imageEffect::EBitDepth& outBitDepth, ofx::imageEffect::EPixelComponent& outComponents)
    {
        outData = getPixelData();
        const OfxRectI bounds = getBounds();
        outWidth = bounds.x2 - bounds.x1;
        outHeight = bounds.y2 - bounds.y1;
        outRowSizeBytes = getRowBytes();
        outBitDepth = getBitDepth();
        outComponents = getComponentsType();
    }

    template <class VIEW_T>
    VIEW_T getGilView(const EImageOrientation orientation);

    template <class VIEW_T>
    VIEW_T getGilView();

public:
#if(TUTTLE_PNG_EXPORT_BETWEEN_NODES)
    void debugSaveAsPng(const std::string& filename);
#endif

private:
    template <class S_VIEW>
    static void copy(Image* dst, S_VIEW& src, const OfxPointI& dstCorner, const OfxPointI& srcCorner,
                     const OfxPointI& count);
    template <class D_VIEW, class S_VIEW>
    static void copy(D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner, const OfxPointI& srcCorner,
                     const OfxPointI& count);
};

template <class VIEW_T>
VIEW_T Image::getGilView(const EImageOrientation orientation)
{
    // const OfxRectI rod = this->getROD();
    const OfxRectI bounds = this->getBounds();

    TUTTLE_LOG_VAR(TUTTLE_TRACE, bounds);
    TUTTLE_LOG_VAR(TUTTLE_TRACE, std::abs(bounds.x2 - bounds.x1));
    TUTTLE_LOG_VAR(TUTTLE_TRACE, std::abs(bounds.y2 - bounds.y1));
    TUTTLE_LOG_VAR(TUTTLE_TRACE, this->getRowBytes());

    typedef typename VIEW_T::value_type Pixel;
    return boost::gil::interleaved_view(std::abs(bounds.x2 - bounds.x1), std::abs(bounds.y2 - bounds.y1),
                                        (Pixel*)(this->getOrientedPixelData(orientation)),
                                        this->getOrientedRowDistanceBytes(orientation));
}

template <class VIEW_T>
VIEW_T Image::getGilView()
{
    // const OfxRectI rod = this->getROD();
    const OfxRectI bounds = this->getBounds();

    typedef typename VIEW_T::value_type Pixel;
    return boost::gil::interleaved_view(std::abs(bounds.x2 - bounds.x1), std::abs(bounds.y2 - bounds.y1),
                                        (Pixel*)(this->getPixelData()), this->getRowAbsDistanceBytes());
}
}
}
}

#endif
