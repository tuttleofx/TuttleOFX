#include "SelectionAverage.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <terry/algorithm/transform_pixels.hpp>


namespace tuttle {
namespace plugin {
namespace colorCubeViewer {
	
//Constructor
SelectionAverage::SelectionAverage(OfxTime time)
{
	//initialize averageValue to 0
	_averageValue.x = _averageValue.y = _averageValue.z = 0;
	_time = time;	//get current time
}


/**
 * Draw average on screen (cross)
 */
void SelectionAverage::draw()
{
	float kCrossSize = 0.05f;
	//compute complementary color
	float complementaryColor[3]; 
	complementaryColor[0] = 1-_averageValue.x;	//complementary red
	complementaryColor[1] = 1-_averageValue.y;	//complementary green
	complementaryColor[2] = 1-_averageValue.z;	//complementary blue
	
	//compute values on X axis
	float xBefore = _averageValue.x-kCrossSize;	//compute before value (X axis)
	float xAfter = _averageValue.x + kCrossSize;	//compute after value (X axis)
	
	//compute values on Y axis
	float yBefore = _averageValue.y-kCrossSize;	//compute before value (Y axis)
	float yAfter = _averageValue.y + kCrossSize;	//compute after value (Y axis)
	
	//compute values on Z axis
	float zBefore = _averageValue.z-kCrossSize;	//compute before value (Z axis)
	float zAfter = _averageValue.z+kCrossSize;	//compute after value (Z axis)
	
	//drawing average mark
	glBegin(GL_LINES);
	glColor3f(complementaryColor[0],complementaryColor[1],complementaryColor[2]);								//color : complementary to average
	glVertex3f(xBefore,_averageValue.y,_averageValue.z); glVertex3f(xAfter,_averageValue.y,_averageValue.z);	//X axis
	glVertex3f(_averageValue.x,yBefore,_averageValue.z); glVertex3f(_averageValue.x,yAfter,_averageValue.z);	//Y axis
	glVertex3f(_averageValue.x,_averageValue.y,zBefore); glVertex3f(_averageValue.x,_averageValue.y,zAfter);	//Z axis
	glEnd();
}

/*
 *Generate average from color selection clip
 */
bool SelectionAverage::computeAverageSelection(OFX::Clip* clipColor, const OfxPointD& renderScale)
{
	// connection test
	if( ! clipColor->isConnected() )
	{	
		return false;
	}
	boost::scoped_ptr<OFX::Image> src( clipColor->fetchImage(_time, clipColor->getCanonicalRod(_time, renderScale)) );	//scoped pointer of current source clip

	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		std::cout << "color src is not accessible (average)" << std::endl;
		return false;
	}

	if( src->getRowDistanceBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		return false;
	}

	const OfxRectI srcPixelRod = clipColor->getPixelRod( _time, renderScale ); //get current RoD
	if( (clipColor->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipColor->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return false;
	}

	//TUTTLE_LOG_VAR( TUTTLE_INFO, src->getBounds());
	//TUTTLE_LOG_VAR( TUTTLE_INFO, src->getRegionOfDefinition() );

	if( srcPixelRod != src->getBounds() )// the host does bad things !
	{
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return false;
	}

	// Compute if source is OK
	SView colorView = tuttle::plugin::getGilView<SView>( src.get(), srcPixelRod, eImageOrientationIndependant );		// get current view from color clip
	ComputeAverage<SView>::CPixel average = ComputeAverage<SView>()( colorView );	// compute color clip average
	
	//copy computed average into average stock variable
	_averageValue.x = average[0]; //red channel value
	_averageValue.y = average[1]; //green channel value
	_averageValue.z = average[2]; //blue channel values
	
	return true; //average has been computed
}

/*
 * Extend a geodesicForm using the selection
 */
void SelectionAverage::extendGeodesicForm(OFX::Clip* clipColor, const OfxPointD& renderScale, GeodesicForm& geodesicForm)
{
	// connection test
	if( ! clipColor->isConnected() )
	{	
		return;
	}

	boost::scoped_ptr<OFX::Image> src( clipColor->fetchImage(_time, clipColor->getCanonicalRod(_time, renderScale)) );	//scoped pointer of current source clip
	
	//TUTTLE_LOG_VAR( TUTTLE_INFO, clipColor->getPixelRod(_time,renderScale)); 
	//TUTTLE_LOG_VAR( TUTTLE_INFO, clipColor->getCanonicalRod(_time, renderScale));

	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		std::cout << "src is not accessible (color clip)" << std::endl;
		return;
	}

	if( src->getRowDistanceBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		return;
	}

	const OfxRectI srcPixelRod = clipColor->getPixelRod( _time, renderScale ); //get current RoD
	if( (clipColor->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipColor->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return;
	}

	if( srcPixelRod != src->getBounds() )// the host does bad things !
	{
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return;
	}

	// Compute if source is OK
	SView colorView = tuttle::plugin::getGilView<SView>( src.get(), srcPixelRod, eImageOrientationIndependant );		// get current view from color clip
	Pixel_extend_GeodesicForm funct(geodesicForm);	//functor declaration			//initialize functor
	terry::algorithm::transform_pixels(colorView, funct);									//with functor reference;
}

}
}
}

