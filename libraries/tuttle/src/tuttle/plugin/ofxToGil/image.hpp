#ifndef _TUTTLE_OFXTOGIL_IMAGE_HPP_
#define	_TUTTLE_OFXTOGIL_IMAGE_HPP_

#include <tuttle/plugin/image.hpp>

#include <ofxsImageEffect.h>

#include <boost/gil/utilities.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>


namespace tuttle {
namespace plugin {

/**
 * @brief Return a full gil view of an image.
 *        If we only have a tiled image, we use subimage_view to fit to the rod.
 * @param img the ofx image object
 * @param rod normally we don't need this parameter because we can get it from the image,
 *            but there is a bug in nuke (which return the bounds),
 *            so we need to use the rod of the clip and not from the image.
 */
template<class View>
View getGilView( OFX::Image* img, const OfxRectI& rod, const EImageOrientation orientation )
{
	using namespace boost::gil;
	typedef typename View::value_type Pixel;
	TUTTLE_TCOUT_X( 50, "-" );
	TUTTLE_TCOUT( "getGilView" );

	//	OfxRectI imgrod = img->getRegionOfDefinition(); // bug in nuke returns bounds... not the clip rod with renderscale...
	const OfxRectI bounds = img->getBounds();
//	TUTTLE_COUT_VAR( bounds );
//	TUTTLE_COUT_VAR( imgrod );
//	TUTTLE_COUT_VAR( rod );
	const point2<int> tileSize = point2<int>( bounds.x2 - bounds.x1,
	                                    bounds.y2 - bounds.y1 );

	// Build view
	/**
	 * About image ordering from OpenFX documentation:
	 * 
	 * Images are always left to right, bottom to top,
	 * with the pixel data pointer being at the bottom left of the image.
	 * The pixels in a scan line are contiguously packed.
	 * Scanlines need not be contiguously packed.
	 * The number of bytes between between a pixel in the same column,
	 * but separated by a scan line is known as the rowbytes of an image.
	 * Rowbytes can be negative, allowing for compositing systems with a native
	 * top to bottom scanline order to trivially support bottom to top images.
	 */
	View tileView = interleaved_view( tileSize.x, tileSize.y,
										static_cast<Pixel*>( img->getPixelData() ),
										img->getRowDistanceBytes() );
	
	TUTTLE_TCOUT_VAR( img->getRowDistanceBytes() );
	TUTTLE_TCOUT_VAR( tileView.pixels().row_size() );
	
	View fullView;
	const bool isTile = (
		bounds.x1 != rod.x1 || bounds.y1 != rod.y1 ||
	    bounds.x2 != rod.x2 || bounds.y2 != rod.y2 );
	// if the tile is equals to the full image
	// directly return the tile
	if( ! isTile )
	{
		TUTTLE_TCOUT( "Tile is equal to the full view" );
		fullView = tileView;
	}
	else
	{
		// view the tile as a full image
		TUTTLE_TCOUT( "Tile to full view" );
		fullView = subimage_view( tileView, rod.x1 - bounds.x1, rod.y1 - bounds.y1, rod.x2 - rod.x1, rod.y2 - rod.y1 );
	}
	
	TUTTLE_TCOUT_VAR( fullView.pixels().row_size() );
	
	View resView = fullView;
	switch( orientation )
	{
		case eImageOrientationIndependant: // use memory order
		{
			TUTTLE_TCOUT( "eImageOrientationIndependant" );
			if( isTile ) // can't manage ordering
				break;

			if( img->getRowDistanceBytes() < 0 ) // if the host use buffer ordered from top to bottom
			{
				resView = flipped_up_down_view( fullView );
			}
			break;
		}
		case eImageOrientationFromTopToBottom:
		{
			TUTTLE_TCOUT( "eImageOrientationFromTopToBottom" );
			BOOST_ASSERT( ! isTile ); // can't manage ordering with tiles currently (no RoW information in OpenFX)

			resView = flipped_up_down_view( fullView );
			break;
		}
		case eImageOrientationFromBottomToTop:
		{
			TUTTLE_TCOUT( "eImageOrientationFromBottomToTop => do nothing" );
			// by default in OpenFX we are in this order
			break;
		}
	}
	
	
	TUTTLE_TCOUT_VAR( resView.pixels().row_size() );

	TUTTLE_TCOUT_X( 50, "-" );
	return resView;
}


}
}



#endif

