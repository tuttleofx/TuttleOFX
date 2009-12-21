#ifndef TUTTLE_HOST_CORE_IMAGE_HPP
#define TUTTLE_HOST_CORE_IMAGE_HPP

#include "ImageEffectNode.hpp"

#include <tuttle/host/ofx/OfxhImage.hpp>
#include <tuttle/host/core/memory/IMemoryPool.hpp>

#include <boost/cstdint.hpp>

namespace tuttle {
namespace host {
namespace core {

class ClipImage;

/**
 * make an image up
 */
class Image : public tuttle::host::ofx::imageEffect::OfxhImage
{
protected:
	size_t _ncomp; ///< number of components
	IPoolDataPtr _data; ///< where we are keeping our image data
	IMemoryPool& _memoryPool;

public:
	explicit Image( ClipImage& clip, const OfxRectD& bounds, OfxTime t );
	virtual ~Image();

	boost::uint8_t* getPixelData() { return reinterpret_cast<boost::uint8_t*>( _data->data() ); }

	boost::uint8_t* pixel( int x, int y );
	static void     copy( Image* dst, Image* src, const OfxPointI& dstCorner,
	                      const OfxPointI& srcCorner, const OfxPointI& count );

	/// @todo use const reference and no pointer !
	template < class VIEW_T >
	static VIEW_T gilViewFromImage( Image* img );

public:
	void debugSaveAsPng( const std::string& filename );

private:
	template < class S_VIEW >
	static void copy( Image* dst, S_VIEW& src, const OfxPointI& dstCorner,
	                  const OfxPointI& srcCorner, const OfxPointI& count );
	template < class D_VIEW, class S_VIEW >
	static void copy( D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner,
	                  const OfxPointI& srcCorner, const OfxPointI& count );
};

}
}
}

#endif
