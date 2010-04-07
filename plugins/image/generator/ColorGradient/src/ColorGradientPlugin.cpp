#include "ColorGradientPlugin.hpp"
#include "ColorGradientProcess.hpp"
#include "ColorGradientAlgorithm.hpp"
#include "ColorGradientDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/algorithm/string/predicate.hpp>
//#include <boost/algorithm/string/classification.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradient {

using namespace boost::gil;

ColorGradientPlugin::ColorGradientPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );

	_gradientType = fetchChoiceParam( kGradientType );
	_nbPoints = fetchIntParam( kNbPoints );

	_points.reserve( kMaxNbPoints );
	_colors.reserve( kMaxNbPoints );
	for( unsigned int i = 0; i < kMaxNbPoints; ++i)
	{
		_points.push_back( fetchDouble2DParam( getPointParamName(i) ) );
		_colors.push_back( fetchRGBAParam( getColorParamName(i) ) );
	}
	OFX::InstanceChangedArgs args;
	args.time = 0;
	args.renderScale.x = 1;
	args.renderScale.y = 1;
	args.reason = OFX::eChangePluginEdit;

	changedParam( args, kNbPoints ); // init IsSecret property for each point/color param
}

template<class View>
ColorGradientProcessParams<View> ColorGradientPlugin::getProcessParams() const
{
	ColorGradientProcessParams<View> params;

	Double2DParamVector::const_iterator it_point = _points.begin();
	RGBAParamVector::const_iterator it_color = _colors.begin();
	unsigned int nbPoints = boost::numeric_cast<unsigned int>( _nbPoints->getValue() );
	for( unsigned int i = 0;
	     i < nbPoints;
	     ++i, ++it_point, ++it_color )
	{
		OfxPointD p = (*it_point)->getValue();
		params._points.push_back( Point2( p.x, p.y ) );
		OfxRGBAColourD c = (*it_color)->getValue();
		params._colors.push_back( rgba32f_pixel_t( c.r, c.g, c.b, c.a ) );
	}
	return params;
}

template<template<typename> class Functor>
void ColorGradientPlugin::renderFunctor( const OFX::RenderArguments &args )
{
//    // instantiate the render code based on the pixel depth of the dst clip
//    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth( );
//    OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents( );
//
//    // do the rendering
//    if( dstComponents == OFX::ePixelComponentRGBA )
//    {
//        switch( dstBitDepth )
//        {
//            case OFX::eBitDepthUByte :
//            {
//                ColorGradientProcess<rgba8_view_t, Functor> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                ColorGradientProcess<rgba16_view_t, Functor> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
                ColorGradientProcess<rgba32f_view_t, Functor> p( *this );
                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthNone :
//                COUT_FATALERROR( "BitDepthNone not recognize." );
//                return;
//            case OFX::eBitDepthCustom :
//                COUT_FATALERROR( "BitDepthCustom not recognize." );
//                return;
//        }
//    }
//    else if( dstComponents == OFX::ePixelComponentAlpha )
//    {
//        switch( dstBitDepth )
//        {
//            case OFX::eBitDepthUByte :
//            {
//                ColorGradientProcess<gray8_view_t, Functor> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                ColorGradientProcess<gray16_view_t, Functor> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                ColorGradientProcess<gray32f_view_t, Functor> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthNone :
//                COUT_FATALERROR( "BitDepthNone not recognize." );
//                return;
//            case OFX::eBitDepthCustom :
//                COUT_FATALERROR( "BitDepthCustom not recognize." );
//                return;
//        }
//    }
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorGradientPlugin::render( const OFX::RenderArguments &args )
{
	switch( static_cast<EGradientType>( _gradientType->getValue() ) )
	{
		case eGradientType1DLinear:
			renderFunctor<ColorGrandient1DLinearFunctor>( args );
			break;
		case eGradientType1DRadial:
			renderFunctor<ColorGrandient1DLinearFunctor>( args ); /// @todo tuttle: not implemented yet
			break;
		case eGradientType2D:
			renderFunctor<ColorGrandient2DLinearFunctor>( args );
			break;
	}
}

void ColorGradientPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kNbPoints )
	{
		unsigned int nbPoints = boost::numeric_cast<unsigned int>( _nbPoints->getValue() );
		Double2DParamVector::iterator it_point = _points.begin();
		RGBAParamVector::iterator it_color = _colors.begin();
		for( unsigned int i = 0; i < nbPoints; ++i, ++it_point, ++it_color )
		{
			(*it_point)->setIsSecret( false );
			(*it_color)->setIsSecret( false );
		}
		for( unsigned int i = nbPoints; i < kMaxNbPoints; ++i, ++it_point, ++it_color )
		{
			(*it_point)->setIsSecret( true );
			(*it_color)->setIsSecret( true );
		}
	}
	else if( boost::starts_with( paramName, kPoint ) )
	{
		try
		{
			unsigned int n = boost::lexical_cast<unsigned int>( paramName.c_str() + kPoint.size() );
			OFX::Double2DParam* param = _points[n];
			OfxPointD p = param->getValue();
			if( n < 2 )
			{
				// A, B
			}
			else
			{
				//
			}
		}
		catch( boost::bad_lexical_cast& )
		{
		}
	}

}

}
}
}
