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
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_sFilename = fetchStringParam( kInputFilename );
}

OFX::Clip* LutPlugin::getSrcClip() const
{
	return _clipSrc;
}

OFX::Clip* LutPlugin::getDstClip() const
{
	return _clipDst;
}

const LutReader & LutPlugin::lutReader() const {
	return _lutReader;
}

LutReader & LutPlugin::lutReader() {
	return _lutReader;
}

Lut3D & LutPlugin::lut3D() {
    return _lut3D;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LutPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = _clipDst->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents();

	if (!_lutReader.readOk()) {
		std::string str;
		_sFilename->getValue( str );
		if( exists( str ) )
		{
			if ( _lutReader.read( str ) )
				COUT_ERROR( "Unable to read lut file..." );
            else
                _lut3D.reset( new TetraInterpolator(), _lutReader );
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
				BOOST_THROW_EXCEPTION( exception::File() << exception::user( "Unable to read lut file..." ) );
            else
                _lut3D.reset( new TetraInterpolator(), _lutReader );
		}
	}
}

}
}
}
