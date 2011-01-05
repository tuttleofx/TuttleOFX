#ifndef TUTTLE_HOST_CORE_IMAGE_HPP
#define TUTTLE_HOST_CORE_IMAGE_HPP

#include <tuttle/host/ofx/OfxhImage.hpp>

#include <tuttle/host/memory/IMemoryPool.hpp>

/// @tuttle: remove include dependencies to gil
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <boost/cstdint.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ClipImage;

/**
 * make an image up
 */
class Image : public tuttle::host::ofx::imageEffect::OfxhImage
{
protected:
	std::size_t _ncomp; ///< number of components
	std::size_t _memlen; ///< memory size
	std::size_t _rowlen; ///< memory size for 1 row
	std::string _fullname;
	memory::IPoolDataPtr _data; ///< where we are keeping our image data

public:
	explicit Image( ClipImage& clip, const OfxRectD& bounds, const OfxTime t );
	virtual ~Image();

	void setPoolData( const memory::IPoolDataPtr& pData )
	{
		_data = pData;
		setPointerProperty( kOfxImagePropData, getPixelData() );
	}

	std::string getFullName() const { return _fullname; }

	std::size_t getMemlen() const { return _memlen; }
	std::size_t getRowlen() const { return _rowlen; }
	
	boost::uint8_t* getPixelData() { return reinterpret_cast<boost::uint8_t*>( _data->data() ); }

	boost::uint8_t* pixel( int x, int y );
	
	/**
	 * @todo clean this!
	 * move outside from class or use copyFrom (don't specify dst)
	 * use ref, change order, etc.
	 */
	static void     copy( Image* dst, Image* src, const OfxPointI& dstCorner,
	                      const OfxPointI& srcCorner, const OfxPointI& count );

	template < class VIEW_T >
	VIEW_T getGilView();

public:
	#ifndef TUTTLE_PRODUCTION
	void debugSaveAsPng( const std::string& filename );
	#endif

private:
	template < class S_VIEW >
	static void copy( Image* dst, S_VIEW& src, const OfxPointI& dstCorner,
	                  const OfxPointI& srcCorner, const OfxPointI& count );
	template < class D_VIEW, class S_VIEW >
	static void copy( D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner,
	                  const OfxPointI& srcCorner, const OfxPointI& count );
};


template < class VIEW_T >
VIEW_T Image::getGilView()
{
	OfxRectI rod = this->getROD();
	OfxRectI bounds = this->getBounds();

	TUTTLE_TCOUT_VAR( bounds );
	TUTTLE_TCOUT_VAR( std::abs( bounds.x2 - bounds.x1 ) );
	TUTTLE_TCOUT_VAR( std::abs( bounds.y2 - bounds.y1 ) );
//	TUTTLE_TCOUT_VAR( this->getPixelData() );
	TUTTLE_TCOUT_VAR( this->getRowBytes() );
	
	typedef typename VIEW_T::value_type Pixel;
	return boost::gil::interleaved_view( std::abs( bounds.x2 - bounds.x1 ),
	                         std::abs( bounds.y2 - bounds.y1 ),
	                         ( Pixel* )( this->getPixelData() ),
	                         this->getRowBytes() );
}


}
}
}

#endif
