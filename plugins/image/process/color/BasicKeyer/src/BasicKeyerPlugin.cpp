#include "BasicKeyerPlugin.hpp"
#include "BasicKeyerProcess.hpp"
#include "BasicKeyerAlgorithm.hpp"
#include "BasicKeyerDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/algorithm/string/predicate.hpp>
//#include <boost/algorithm/string/classification.hpp>

namespace tuttle {
namespace plugin {
namespace basicKeyer {

using namespace boost::gil;

BasicKeyerPlugin::BasicKeyerPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramMode     = fetchChoiceParam( kParamMode );
	_paramNbPoints = fetchIntParam( kParamNbPoints );

	_paramPoints.reserve( kMaxNbPoints );
	_paramColors.reserve( kMaxNbPoints );
	for( unsigned int i = 0; i < kMaxNbPoints; ++i )
	{
		_paramPoints.push_back( fetchDouble2DParam( getPointParamName( i ) ) );
		_paramColors.push_back( fetchRGBAParam( getColorParamName( i ) ) );
	}

	_instanceChangedArgs.time          = 0;
	_instanceChangedArgs.renderScale.x = 1;
	_instanceChangedArgs.renderScale.y = 1;
	_instanceChangedArgs.reason        = OFX::eChangePluginEdit;
	changedParam( _instanceChangedArgs, kParamNbPoints ); // init IsSecret property for each point/color param
}

template<class View>
BasicKeyerProcessParams<View> BasicKeyerPlugin::getProcessParams() const
{
	BasicKeyerProcessParams<View> params;

	Double2DParamVector::const_iterator it_point = _paramPoints.begin();
	RGBAParamVector::const_iterator it_color     = _paramColors.begin();
	unsigned int nbPoints                        = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
	for( unsigned int i = 0;
	     i < nbPoints;
	     ++i, ++it_point, ++it_color )
	{
		OfxPointD p = ( *it_point )->getValue();
		params._points.push_back( Point2( p.x, p.y ) );
		OfxRGBAColourD c = ( *it_color )->getValue();
		params._colors.push_back( rgba32f_pixel_t( c.r, c.g, c.b, c.a ) );
	}
	return params;
}

/*
   void BasicKeyerPlugin::process()
   {
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );
    switch( dstComponents )
    {
        case OFX::ePixelComponentRGBA:
            processComponents<rgba_t>();
            return;
        case OFX::ePixelComponentAlpha:
            processComponents<gray_t>();
            return;
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
            return;
    }
   }

   template<typename ColorSpace>
   void BasicKeyerPlugin::processComponents()
   {
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    switch( dstBitDepth )
    {
        case OFX::eBitDepthUByte :
        {
            processBitDepth<ColorSpace, bits8_t> p( *this );
            return;
        }
        case OFX::eBitDepthUShort :
        {
            processBitDepth<ColorSpace, bits16_t> p( *this );
            return;
        }
        case OFX::eBitDepthFloat :
        {
            processBitDepth<ColorSpace, bits32f_t> p( *this );
            return;
        }
        case OFX::eBitDepthNone :
            TUTTLE_COUT_FATALERROR( "BitDepthNone not recognize." );
            return;
        case OFX::eBitDepthCustom :
            TUTTLE_COUT_FATALERROR( "BitDepthCustom not recognize." );
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
void BasicKeyerPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<BasicKeyerProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void BasicKeyerPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamNbPoints )
	{
		const unsigned int nbPoints            = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
		Double2DParamVector::iterator it_point = _paramPoints.begin();
		RGBAParamVector::iterator it_color     = _paramColors.begin();
		for( unsigned int i = 0; i < nbPoints; ++i, ++it_point, ++it_color )
		{
			( *it_point )->setIsSecret( false );
			( *it_color )->setIsSecret( false );
		}
		for( unsigned int i = nbPoints; i < kMaxNbPoints; ++i, ++it_point, ++it_color )
		{
			( *it_point )->setIsSecret( true );
			( *it_color )->setIsSecret( true );
		}
		changedParam( _instanceChangedArgs, kParamMode );
	}
	else if( paramName == kParamMode )
	{
		const unsigned int nbPoints            = boost::numeric_cast<unsigned int>( _paramNbPoints->getValue() );
		EParamMode mode                        = static_cast<EParamMode>( _paramMode->getValue() );
		Double2DParamVector::iterator it_point = _paramPoints.begin();
		RGBAParamVector::iterator it_color     = _paramColors.begin();
		switch( mode )
		{
			case eParamModeColor:
				for( unsigned int i = 0; i < kMaxNbPoints; ++i, ++it_point, ++it_color )
				{
					( *it_point )->setIsSecret( true );
					( *it_color )->setEnabled( true );
				}
				break;
			case eParamModePosition:
				for( unsigned int i = 0; i < kMaxNbPoints; ++i, ++it_color )
				{
					( *it_color )->setEnabled( false );
				}
				for( unsigned int i = 0; i < nbPoints; ++i, ++it_point )
				{
					( *it_point )->setIsSecret( false );
				}
				break;
		}
	}
	else if( boost::starts_with( paramName, kParamPoint ) )
	{
		try
		{
			unsigned int n            = boost::lexical_cast<unsigned int>( paramName.c_str() + kParamPoint.size() );
			OFX::Double2DParam* param = _paramPoints[n];
			OfxPointD p               = param->getValue();
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
		{}
	}

}

}
}
}
