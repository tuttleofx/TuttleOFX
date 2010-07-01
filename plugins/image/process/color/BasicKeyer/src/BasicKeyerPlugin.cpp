#include "BasicKeyerPlugin.hpp"
#include "BasicKeyerProcess.hpp"
#include "BasicKeyerAlgorithm.hpp"
#include "BasicKeyerDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/algorithm/string/predicate.hpp>
//#include <boost/algorithm/string/classification.hpp>

namespace tuttle {
namespace plugin {
namespace basicKeyer {

using namespace boost::gil;

BasicKeyerPlugin::BasicKeyerPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramMode = fetchChoiceParam( kParamMode );
	_paramNbPoints = fetchIntParam( kParamNbPoints );

	_paramPoints.reserve( kMaxNbPoints );
	_paramColors.reserve( kMaxNbPoints );
	for( unsigned int i = 0; i < kMaxNbPoints; ++i )
	{
		_paramPoints.push_back( fetchDouble2DParam( getPointParamName(i) ) );
		_paramColors.push_back( fetchRGBAParam( getColorParamName(i) ) );
	}

	_instanceChangedArgs.time = 0;
	_instanceChangedArgs.renderScale.x = 1;
	_instanceChangedArgs.renderScale.y = 1;
	_instanceChangedArgs.reason = OFX::eChangePluginEdit;
	changedParam( _instanceChangedArgs, kParamNbPoints ); // init IsSecret property for each point/color param
}

template<class View>
BasicKeyerProcessParams<View> BasicKeyerPlugin::getProcessParams() const
{
	BasicKeyerProcessParams<View> params;

	Double2DParamVector::const_iterator it_point = _paramPoints.begin();
	RGBAParamVector::const_iterator it_color = _paramColors.begin();
	unsigned int nbPoints = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
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

/*
void BasicKeyerPlugin::process()
{
    OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents( );
    switch( dstComponents )
    {
		case OFX::ePixelComponentRGBA:
			processComponents<rgba_t>();
			break;
		case OFX::ePixelComponentAlpha:
			processComponents<gray_t>();
			break;
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
			break;
	}
}

template<typename ColorSpace>
void BasicKeyerPlugin::processComponents()
{
    OFX::BitDepthEnum dstBitDepth = _clipDst->getPixelDepth( );
	switch( dstBitDepth )
	{
		case OFX::eBitDepthUByte :
		{
			processBitDepth<ColorSpace, bits8_t> p( *this );
			break;
		}
		case OFX::eBitDepthUShort :
		{
			processBitDepth<ColorSpace, bits16_t> p( *this );
			break;
		}
		case OFX::eBitDepthFloat :
		{
			processBitDepth<ColorSpace, bits32f_t> p( *this );
			break;
		}
		case OFX::eBitDepthNone :
			COUT_FATALERROR( "BitDepthNone not recognize." );
			return;
		case OFX::eBitDepthCustom :
			COUT_FATALERROR( "BitDepthCustom not recognize." );
			return;
	}
}

template<typename ColorSpace, typename BitDepth>
void BasicKeyerPlugin::processBitDepth()
{
	typedef pixel<BitDepth, layout<ColorSpace> > Pixel;
	typedef view_from_pixel<Pixel>::type View;

	BasicKeyerProcess<View> p( *this );
	p.setupAndProcess( args );
}
*/

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BasicKeyerPlugin::render( const OFX::RenderArguments &args )
{
//    // instantiate the render code based on the pixel depth of the dst clip
//    OFX::BitDepthEnum dstBitDepth = _clipDst->getPixelDepth( );
//    OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents( );
//
//    // do the rendering
//    if( dstComponents == OFX::ePixelComponentRGBA )
//    {
//        switch( dstBitDepth )
//        {
//            case OFX::eBitDepthUByte :
//            {
//                BasicKeyerProcess<rgba8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                BasicKeyerProcess<rgba16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
                BasicKeyerProcess<rgba32f_view_t> p( *this );
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
//                BasicKeyerProcess<gray8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                BasicKeyerProcess<gray16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                BasicKeyerProcess<gray32f_view_t> p( *this );
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

void BasicKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamNbPoints )
	{
		const unsigned int nbPoints = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
		Double2DParamVector::iterator it_point = _paramPoints.begin();
		RGBAParamVector::iterator it_color = _paramColors.begin();
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
		changedParam( _instanceChangedArgs, kParamMode );
	}
	else if( paramName == kParamMode )
	{
		const unsigned int nbPoints = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
		EParamMode mode = static_cast<EParamMode>(_paramMode->getValue());
		Double2DParamVector::iterator it_point = _paramPoints.begin();
		RGBAParamVector::iterator it_color = _paramColors.begin();
		switch( mode )
		{
			case eParamModeColor:
				for( unsigned int i = 0; i < kMaxNbPoints; ++i, ++it_point, ++it_color )
				{
					(*it_point)->setIsSecret( true );
					(*it_color)->setEnabled( true );
				}
				break;
			case eParamModePosition:
				for( unsigned int i = 0; i < kMaxNbPoints; ++i, ++it_color )
				{
					(*it_color)->setEnabled( false );
				}
				for( unsigned int i = 0; i < nbPoints; ++i, ++it_point )
				{
					(*it_point)->setIsSecret( false );
				}
				break;
		}
	}
	else if( boost::starts_with( paramName, kParamPoint ) )
	{
		try
		{
			unsigned int n = boost::lexical_cast<unsigned int>( paramName.c_str() + kParamPoint.size() );
			OFX::Double2DParam* param = _paramPoints[n];
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
