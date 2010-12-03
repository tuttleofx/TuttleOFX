#include "FloodFillAlgorithm.hpp"
#include "FloodFillPlugin.hpp"

#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {

template<class View>
FloodFillProcess<View>::FloodFillProcess( FloodFillPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void FloodFillProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void FloodFillProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	
	static const unsigned int border = 1;
	OfxRectI srcRodCrop = rectangleReduce( this->_srcPixelRod, border );
	OfxRectI procWindowRoWCrop = rectanglesIntersection( procWindowRoW, srcRodCrop );

	fill_pixels( this->_dstView, procWindowOutput, get_black<Pixel>() );

	flood_fill<Connectivity4>( this->_srcView, this->_srcPixelRod,
	                           this->_dstView, this->_dstPixelRod,
	                           procWindowRoWCrop,
	                           IsUpper<Scalar>(_params._upperThres),
	                           IsUpper<Scalar>(_params._lowerThres)
	                          );

}

}
}
}
