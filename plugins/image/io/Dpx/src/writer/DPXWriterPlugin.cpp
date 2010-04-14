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
	_srcClip        = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip        = fetchClip( kOfxImageEffectOutputClipName );
	_filepath       = fetchStringParam( kOutputFilename );
	_bitDepth       = fetchChoiceParam( kParamPrecision );
	_componentsType = fetchChoiceParam( kParamComponentsType );
	_compressed     = fetchBooleanParam( kParamCompressed );
	_renderButton   = fetchPushButtonParam( kRender );
	_renderAlways   = fetchBooleanParam( kParamRenderAlways );
}

OFX::Clip* DPXWriterPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* DPXWriterPlugin::getDstClip() const
{
	return _dstClip;
}

DPXWriterParams DPXWriterPlugin::getParams(const OfxTime time)
{
	DPXWriterParams params;
	params._bitDepth = _bitDepth->getValue();
	params._componentsType = _componentsType->getValue();
	params._compressed = _compressed->getValue();
	params._filepath = _fPattern.getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _renderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
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
	}
}

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kOutputFilename )
	{
		_fPattern.reset(_filepath->getValue(), false, 0, 1);
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
