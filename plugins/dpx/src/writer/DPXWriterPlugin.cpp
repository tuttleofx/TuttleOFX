/**
 * @file DPXWriterPlugin.cpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_srcClip      = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip      = fetchClip( kOfxImageEffectOutputClipName );
	_filepath     = fetchStringParam( kOutputFilename );
	_renderButton = fetchPushButtonParam( kRender );
}

OFX::Clip* DPXWriterPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* DPXWriterPlugin::getDstClip() const
{
	return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _bRenderOnce )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _dstClip->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					DPXWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					DPXWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					DPXWriterProcess<rgba32f_view_t> fred( *this );
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

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	_bRenderOnce = false;
	if( paramName == kRender )
	{
		_bRenderOnce = true;    // Hack stuff...
	}
	else if( paramName == kOutputFilename )
	{
		std::string str;
		_filepath->getValue( str );
		_bRenderOnce = true;
	}
	else if( paramName == kDPXWriterHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kDPXWriterHelpString );
	}
}

}
}
}
}
