#include "Image.hpp"
#include "Core.hpp"
#include "attribute/ClipImage.hpp"

// utilities
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

//TODO: delete this
#ifdef _DEBUG
 #include <boost/gil/extension/io/png_io.hpp>
#endif

namespace tuttle {
namespace host {
namespace core {

Image::Image( ClipImage& clip, const OfxRectD& bounds, OfxTime time )
	: ofx::imageEffect::OfxhImage( clip ) ///< this ctor will set basic props on the image
	, _memoryPool( Core::instance().getMemoryPool() )
{
	size_t memlen = 0;
	size_t rowlen = 0;

	_ncomp = 0;
	// Set rod in canonical & pixel coord.
	OfxRectI ibounds;
	double par = clip.getPixelAspectRatio();
	ibounds.x1 = int(bounds.x1 / par);
	ibounds.x2 = int(bounds.x2 / par);
	ibounds.y1 = int(bounds.y1);
	ibounds.y2 = int(bounds.y2);

	OfxPointI dimensions = { ibounds.x2 - ibounds.x1, ibounds.y2 - ibounds.y1 };

	if( clip.getComponents() == kOfxImageComponentRGBA )
	{
		_ncomp = 4;
	}
	else if( clip.getComponents() == kOfxImageComponentAlpha )
	{
		_ncomp = 1;
	}

	// make some memory according to the bit depth
	if( clip.getPixelDepth() == kOfxBitDepthByte )
	{
		memlen = _ncomp * dimensions.x * dimensions.y;
		rowlen = _ncomp * dimensions.x;
	}
	else if( clip.getPixelDepth() == kOfxBitDepthShort )
	{
		memlen = _ncomp * dimensions.x * dimensions.y * sizeof( boost::uint16_t );
		rowlen = _ncomp * dimensions.x * sizeof( boost::uint16_t );
	}
	else if( clip.getPixelDepth() == kOfxBitDepthFloat )
	{
		memlen = int(_ncomp * dimensions.x * dimensions.y * sizeof( float ) );
		rowlen = int(_ncomp * dimensions.x * sizeof( float ) );
	}

	_data = _memoryPool.allocate( memlen );
	// now blank it
	//memset( getPixelData(), 0, memlen );

	// render scale x and y of 1.0
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 0 );
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 1 );

	// data ptr
	setPointerProperty( kOfxImagePropData, getPixelData() );

	// bounds and rod
	setIntProperty( kOfxImagePropBounds, ibounds.x1, 0 );
	setIntProperty( kOfxImagePropBounds, ibounds.y1, 1 );
	setIntProperty( kOfxImagePropBounds, ibounds.x2, 2 );
	setIntProperty( kOfxImagePropBounds, ibounds.y2, 3 );

	/// @todo the same for bounds and rod, no tiles for the moment !
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.x1, 0 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.y1, 1 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.x2, 2 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.y2, 3 );

	// row bytes
	setIntProperty( kOfxImagePropRowBytes, rowlen );
}

Image::~Image()
{}

boost::uint8_t* Image::pixel( int x, int y )
{
	OfxRectI bounds = getBounds();

	if( ( x >= bounds.x1 ) && ( x < bounds.x2 ) && ( y >= bounds.y1 ) && ( y < bounds.y2 ) )
	{
		int rowBytes = getIntProperty( kOfxImagePropRowBytes );
		int offset   = ( y = bounds.y1 ) * rowBytes + ( x - bounds.x1 ) * _ncomp;
		return &( getPixelData()[offset] );
	}
	return 0;
}

template < class VIEW_T >
VIEW_T Image::gilViewFromImage( Image* img )
{
	using namespace bgil;
	OfxRectI bounds = img->getBounds();

	typedef typename VIEW_T::value_type value_t;
	return interleaved_view( std::abs( bounds.x2 - bounds.x1 ),
	                         std::abs( bounds.y2 - bounds.y1 ),
	                         ( value_t* )( img->getPixelData() ),
	                         img->getRowBytes() );
}

/// @todo: put this in gilGlobals.hpp
template < class D_VIEW, class S_VIEW >
void Image::copy( D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	using namespace bgil;
	if( src.width() >= ( count.x - srcCorner.x ) &&
	    src.height() >= ( count.y - srcCorner.y ) &&
	    dst.width() >= ( count.x - dstCorner.x ) &&
	    dst.height() >= ( count.y - dstCorner.y ) )
	{
		S_VIEW subSrc = subimage_view( src, srcCorner.x, srcCorner.y, count.x, count.y );
		D_VIEW subDst = subimage_view( dst, dstCorner.x, dstCorner.y, count.x, count.y );
		copy_and_convert_pixels( subSrc, subDst );
	}
}

#ifdef _DEBUG
void Image::debugSaveAsPng( const std::string& filename )
{
	switch( getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t view = gilViewFromImage<rgba8_view_t >( this );
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t view = gilViewFromImage<rgba16_view_t >( this );
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t view = gilViewFromImage<rgba32f_view_t >( this );
					png_write_view( filename, clamp<rgb8_pixel_t>( view ) );
					break;
				}
				default:
					break;
			}
			break;
		case ofx::imageEffect::ePixelComponentAlpha:
			switch( getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					gray8_view_t view = gilViewFromImage<gray8_view_t >( this );
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t view = gilViewFromImage<gray16_view_t >( this );
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t view = gilViewFromImage<gray32f_view_t >( this );
					png_write_view( filename, clamp<rgb8_pixel_t>( view ) );
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

#endif

/// Copy from gil image view to Image
template < class S_VIEW >
void Image::copy( Image* dst, S_VIEW& src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	using namespace bgil;
	// Create destination
	switch( dst->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( dst->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t dView = gilViewFromImage<rgba8_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t dView = gilViewFromImage<rgba16_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t dView = gilViewFromImage<rgba32f_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		case ofx::imageEffect::ePixelComponentAlpha:
			switch( dst->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					gray8_view_t dView = gilViewFromImage<gray8_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t dView = gilViewFromImage<gray16_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t dView = gilViewFromImage<gray32f_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
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
void Image::copy( Image* dst, Image* src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	using namespace bgil;
	switch( src->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( src->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t sView = gilViewFromImage<rgba8_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t sView = gilViewFromImage<rgba16_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t sView = gilViewFromImage<rgba32f_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		case ofx::imageEffect::ePixelComponentAlpha:
			switch( src->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					gray8_view_t sView = gilViewFromImage<gray8_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t sView = gilViewFromImage<gray16_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t sView = gilViewFromImage<gray32f_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
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
