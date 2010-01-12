/**
 * @file LutPlugin.cpp
 * @brief 3D-Lut ofx plugin
 * @author Eloi Du Bois
 * @date    13/11/09 13:42
 *
 */

#include "LutPlugin.hpp"
#include "LutProcess.hpp"
#include "LutDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace lut {

using namespace boost::gil;
const static std::string kLutHelpString = "<b>Image Luter</b> is used to lut components of an image.  <br />";

LutPlugin::LutPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip = fetchClip( kOfxImageEffectOutputClipName );
	_sFilename = fetchStringParam( kInputFilename );
	std::string str;
	_sFilename->getValue( str );
	if( exists( str ) )
	{
		if ( _lutReader.read( str ) )
			COUT_ERROR("Unable to read lut file..." );
	}
}

OFX::Clip* LutPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* LutPlugin::getDstClip() const
{
	return _dstClip;
}

const LutReader & LutPlugin::lutReader() const {
	return _lutReader;
}

LutReader & LutPlugin::lutReader() {
	return _lutReader;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LutPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = _dstClip->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

	if (!_lutReader.readOk()) {
		std::string str;
		_sFilename->getValue( str );
		if( exists( str ) )
		{
			if ( _lutReader.read( str ) )
				COUT_ERROR( "Unable to read lut file..." );
		}
	}
	if (_lutReader.readOk()) {
		// select render according to pixel components & bit depth
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					LutProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					LutProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					LutProcess<rgba32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone:
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom:
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			}
		}
		else if( dstComponents == OFX::ePixelComponentAlpha )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					LutProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					LutProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					LutProcess<gray32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone:
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom:
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			}
		}
	}
}

void LutPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kLutHelpString );
	}
	else if ( paramName == kInputFilename )
	{
		std::string str;
		_sFilename->getValue( str );
		if( exists( str ) )
		{
			if ( _lutReader.read( str ) )
				throw( PluginException( "Unable to read lut file..." ) );
		}
	}
}

}
}
}
