#include "LensDistortOverlayInteract.hpp"
#include "lensDistortDefinitions.hpp"
#include "LensDistortPlugin.hpp"
#include "lensDistortAlgorithm.hpp"
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
#include <tuttle/plugin/interact/ParamPointRelativePoint.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace lens {

/**
 * @brief create a list of points which represents a grid in image coordinates
 * @param imgSize image size (in pixels)
 * @param nbTiles number of tiles
 * @param scale scale of the grid (normalized value)
 * @param center shift to apply on the image (normalized value)
 */
template<typename Point2>
std::vector<std::vector<Point2> > createGrid( const OfxRectD& rod, const unsigned int nbTiles, const Point2& scale, const Point2& center, const unsigned int steps = 50 )
{
	//	Point2 rodCorner( rod.x1, rod.y1 );
	Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	const unsigned int size = nbTiles + 1; // number of lines
	const double stepSize   = rodSize.x / (double) steps;
	const double tileSize   = rodSize.x / (double) nbTiles;
	const double halfSize   = rodSize.x / 2.0;
	const double yShift     = ( rodSize.x - rodSize.y ) / 2.0; // vertical shift (because it's a squared grid)
	const Point2 pixCenter  = ( center * rodSize.x ); // center in pixel coordinates

	std::vector<std::vector<Point2> > lines;
	lines.reserve( 2 * size );
	for( unsigned int i = 0; i < size; ++i )
	{
		std::vector<Point2> horizontal, vertical;
		horizontal.reserve( steps );
		vertical.reserve( steps );
		for( unsigned int j = 0; j <= steps; ++j )
		{
			double si = ( i * tileSize ) - halfSize;
			Point2 diagonal( si * scale.x + halfSize + pixCenter.x, si * scale.y + halfSize + pixCenter.y - yShift );
			double sj = ( j * stepSize ) - halfSize;
			horizontal.push_back( Point2( sj * scale.x + halfSize + pixCenter.x, diagonal.y ) );
			vertical.push_back( Point2( diagonal.x, sj * scale.y + halfSize + pixCenter.y - yShift ) );
		}
		lines.push_back( horizontal );
		lines.push_back( vertical );
	}
	return lines;
}

template<typename Point2>
void shiftGrid( std::vector<std::vector<Point2> >& grid, const Point2& shift )
{
	for( typename std::vector<std::vector<Point2> >::iterator itv = grid.begin(), itvEnd = grid.end();
	     itv != itvEnd;
	     ++itv )
	{
		for( typename std::vector<Point2>::iterator it = itv->begin(), itEnd = itv->end();
		     it != itEnd;
		     ++it  )
		{
			*it += shift;
		}
	}
}

LensDistortOverlayInteract::LensDistortOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = static_cast<LensDistortPlugin*>( _effect );

	const interact::FrameOptionalClip frame( _plugin->_srcRefClip, _plugin->_clipSrc );
	interact::PointInteract* centerPoint = new interact::ParamPoint<interact::FrameOptionalClip, eCoordonateSystemXXcn>( _infos, _plugin->_center, frame );
	_interactScene.push_back( new interact::ParamPointRelativePoint<interact::FrameOptionalClip, eCoordonateSystemXXcn>( _infos, _plugin->_gridCenter, frame, centerPoint ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_gridCenterOverlay ) );
	_interactScene.push_back( centerPoint, new interact::IsActiveBooleanParamFunctor<>( _plugin->_centerOverlay ) );
}

bool LensDistortOverlayInteract::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<Scalar> Point2;
	static const float lineWidth = 2.0;
	bool displaySomething        = false;

	// debug drawing
	if( _plugin->_debugDisplayRoi->getValue() )
	{
		glLineWidth( lineWidth );
		displaySomething = true;

		glColor3f( 1.0f, 0.0f, 0.0f );
		overlay::displayRect( _plugin->_dstRoi, -1 );

		glColor3f( 0.0f, 1.0f, 0.0f );
		overlay::displayRect( _plugin->_srcRoi, 1 );

		glColor3f( 0.0f, 0.0f, 1.0f );
		overlay::displayRect( _plugin->_srcRealRoi, 1 );
	}

	displaySomething |= _interactScene.draw( args );

	// drawing
	if( _plugin->_gridOverlay->getValue() && _plugin->_clipSrc->isConnected() )
	{
		displaySomething = true;
		static const unsigned int steps = 10;

		// get the project size
		OfxRectD srcRod = _plugin->_clipSrc->getCanonicalRod( args.time );
		if( _plugin->_srcRefClip->isConnected() )
			srcRod = _plugin->_srcRefClip->getCanonicalRod( args.time );

		const Point2 imgSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );
		const OfxRectD outputRod = _plugin->_clipDst->getCanonicalRod( args.time );
		//parameters
		const Point2 gridCenter( ofxToGil( _plugin->_gridCenter->getValue() ) );
		const Point2 gridScale( ofxToGil( _plugin->_gridScale->getValue() ) );
		std::vector<std::vector<Point2> > grid = createGrid( srcRod, steps, gridScale, gridCenter );
		if( !_plugin->_displaySource->getValue() )
		{
			LensDistortProcessParams<Scalar> params;
			static const double pixelAspectRatio = 1.0; // here the pixel aspect ratio is 1.0 because we work in canonical coordinates
			params = _plugin->getProcessParams( srcRod, srcRod, pixelAspectRatio, true );
			// work in output clip coordinates
			transformValuesApply( _plugin->getLensType(), params, grid );
		}
		Point2 rodCorner( srcRod.x1, srcRod.y1 );
		shiftGrid( grid, rodCorner ); // to move in RoW coordinates

		glLineWidth( lineWidth );
		glColor3f( 1.0f, 1.0f, 0.0f );
		overlay::drawCurves( grid );
	}
	return displaySomething;
}

bool LensDistortOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool LensDistortOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool LensDistortOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}
