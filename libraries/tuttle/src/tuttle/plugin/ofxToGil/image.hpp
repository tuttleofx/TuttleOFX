#ifndef _TUTTLE_OFXTOGIL_IMAGE_HPP_
#define	_TUTTLE_OFXTOGIL_IMAGE_HPP_

#include <ofxsImageEffect.h>

#include <boost/gil/utilities.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>


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
View getView( OFX::Image* img, const OfxRectI& rod )
{
	using namespace boost::gil;
	typedef typename View::value_type Pixel;

	//	OfxRectI imgrod = img->getRegionOfDefinition(); // bug in nuke returns bounds... not the clip rod with renderscale...
	OfxRectI bounds = img->getBounds();
//	TUTTLE_COUT_VAR( bounds );
//	TUTTLE_COUT_VAR( imgrod );
//	TUTTLE_COUT_VAR( rod );
	point2<int> tileSize = point2<int>( bounds.x2 - bounds.x1,
	                                    bounds.y2 - bounds.y1 );

	// Build views
	View tileView = interleaved_view( tileSize.x, tileSize.y,
	                                  static_cast<Pixel*>( img->getPixelData() ),
	                                  img->getRowBytes() );

	// if the tile is equals to the full image
	// directly return the tile
	if( bounds.x1 == rod.x1 && bounds.y1 == rod.y1 &&
	    bounds.x2 == rod.x2 && bounds.y2 == rod.y2 )
		return tileView;

	// view the tile as a full image
	return subimage_view( tileView, rod.x1 - bounds.x1, rod.y1 - bounds.y1, rod.x2 - rod.x1, rod.y2 - rod.y1 );
}


}
}



#endif

