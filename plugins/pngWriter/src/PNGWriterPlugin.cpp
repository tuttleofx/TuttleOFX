/**
 * @file PNGWriterPlugin.cpp
 * @brief PNG writer ofx plugin
 * @author Eloi Du Bois
 * @date    27/09/09 10:42
 *
 */
#include "PNGWriterPlugin.hpp"
#include "PNGWriterProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {

using namespace boost::gil;
const static std::string kPngReaderHelpString = "<b>PNG Reader</b> file reader.  <br />";

PNGWriterPlugin::PNGWriterPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle ),
	_bRenderOnce( false )
{
	_srcClip      = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip      = fetchClip( kOfxImageEffectOutputClipName );
	_filepath     = fetchStringParam( kInputFilename );
	_renderButton = fetchPushButtonParam( kRender );
}

OFX::Clip* PNGWriterPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* PNGWriterPlugin::getDstClip() const
{
	return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PNGWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _bRenderOnce )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _srcClip->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _srcClip->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					PNGWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					PNGWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					PNGWriterProcess<rgba32f_view_t> fred( *this );
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
					PNGWriterProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					PNGWriterProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					PNGWriterProcess<gray32f_view_t> fred( *this );
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
		_bRenderOnce = false;
	}
}

void PNGWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	_bRenderOnce = false;
	if( paramName == kRender )
	{
		_bRenderOnce = true;    // Hack stuff...
	}
	else if( paramName == kInputFilename )
	{
		std::string str;
		_filepath->getValue( str );
		_bRenderOnce = true;
	}
}

}
}
}
