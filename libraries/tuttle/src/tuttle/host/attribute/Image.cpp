#include "Image.hpp"
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/Core.hpp>

#include <tuttle/common/utils/global.hpp>

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#ifndef TUTTLE_PRODUCTION
 #include <boost/gil/extension/io/png_io.hpp>
#endif

namespace tuttle {
namespace host {
namespace attribute {

Image::Image( ClipImage& clip, const OfxRectD& bounds, const OfxTime time )
	: ofx::imageEffect::OfxhImage( clip ) ///< this ctor will set basic props on the image
	, _memlen(0)
	, _rowlen(0)
{
	_ncomp = 0;
	// Set rod in canonical & pixel coord.
	OfxRectI ibounds;
	double par = clip.getPixelAspectRatio();
	ibounds.x1 = int(bounds.x1 / par);
	ibounds.x2 = int(bounds.x2 / par);
	ibounds.y1 = int(bounds.y1);
	ibounds.y2 = int(bounds.y2);

	OfxPointI dimensions = { ibounds.x2 - ibounds.x1, ibounds.y2 - ibounds.y1 };

	if( clip.getComponentsString() == kOfxImageComponentRGBA )
	{
		_ncomp = 4;
	}
	else if( clip.getComponentsString() == kOfxImageComponentAlpha )
	{
		_ncomp = 1;
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()
		    << exception::user() + "Unsupported component type: " + quotes( clip.getComponentsString() ) );
	}

	// make some memory according to the bit depth
	if( clip.getBitDepthString() == kOfxBitDepthByte )
	{
		_memlen = _ncomp * dimensions.x * dimensions.y;
		_rowlen = _ncomp * dimensions.x;
	}
	else if( clip.getBitDepthString() == kOfxBitDepthShort )
	{
		_memlen = _ncomp * dimensions.x * dimensions.y * sizeof( boost::uint16_t );
		_rowlen = _ncomp * dimensions.x * sizeof( boost::uint16_t );
	}
	else if( clip.getBitDepthString() == kOfxBitDepthFloat )
	{
		_memlen = int(_ncomp * dimensions.x * dimensions.y * sizeof( float ) );
		_rowlen = int(_ncomp * dimensions.x * sizeof( float ) );
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()
		    << exception::user() + "Unsupported pixel depth: " + quotes( clip.getBitDepthString() ) );
	}

	// render scale x and y of 1.0
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 0 );
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 1 );

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
	setIntProperty( kOfxImagePropRowBytes, _rowlen );
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
	return NULL;
}

template < class D_VIEW, class S_VIEW >
void Image::copy( D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	using namespace boost::gil;
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

#ifndef TUTTLE_PRODUCTION
void Image::debugSaveAsPng( const std::string& filename )
{
	using namespace boost::gil;
	switch( getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t view = getGilView<rgba8_view_t >();
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t view = getGilView<rgba16_view_t >();
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t view = getGilView<rgba32f_view_t >();
					png_write_view( filename, color_converted_view<rgba8_pixel_t>( view ) );
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
					gray8_view_t view = getGilView<gray8_view_t >();
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t view = getGilView<gray16_view_t >();
					png_write_view( filename, view );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t view = getGilView<gray32f_view_t >();
					png_write_view( filename, color_converted_view<rgb8_pixel_t>( view ) );
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
	using namespace boost::gil;
	// Create destination
	switch( dst->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( dst->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t dView = dst->getGilView<rgba8_view_t >();
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t dView = dst->getGilView<rgba16_view_t >();
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t dView = dst->getGilView<rgba32f_view_t >();
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
					gray8_view_t dView = dst->getGilView<gray8_view_t >();
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t dView = dst->getGilView<gray16_view_t >();
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t dView = dst->getGilView<gray32f_view_t >();
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
	using namespace boost::gil;
	switch( src->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( src->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t sView = src->getGilView<rgba8_view_t >();
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t sView = src->getGilView<rgba16_view_t >();
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t sView = src->getGilView<rgba32f_view_t >();
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
					gray8_view_t sView = src->getGilView<gray8_view_t >();
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t sView = src->getGilView<gray16_view_t >();
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t sView = src->getGilView<gray32f_view_t >();
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
