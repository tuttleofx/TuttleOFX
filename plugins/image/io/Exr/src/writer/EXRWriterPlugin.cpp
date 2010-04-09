/**
 * @file EXRWriterPlugin.cpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#include "EXRWriterPlugin.hpp"
#include "EXRWriterProcess.hpp"
#include "EXRWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {


using namespace boost::gil;

EXRWriterPlugin::EXRWriterPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_srcClip        = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip        = fetchClip( kOfxImageEffectOutputClipName );
	_filepath       = fetchStringParam( kOutputFilename );
	_bitDepth       = fetchChoiceParam( kParamBitDepth );
	_componentsType = fetchChoiceParam( kParamComponentsType );
	_renderButton   = fetchPushButtonParam( kRender );
	_renderAlways   = fetchBooleanParam( kParamRenderAlways );
}

OFX::Clip* EXRWriterPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* EXRWriterPlugin::getDstClip() const
{
	return _dstClip;
}

EXRWriterParams EXRWriterPlugin::getParams(const OfxTime time)
{
	EXRWriterParams params;
	params._bitDepth = (EBitDepth)_bitDepth->getValue();
	params._componentsType = (ECompType)_componentsType->getValue();
	params._filepath = _fPattern.getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRWriterPlugin::render( const OFX::RenderArguments& args )
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
					EXRWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					EXRWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					EXRWriterProcess<rgba32f_view_t> fred( *this );
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

void EXRWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kOutputFilename )
	{
		_fPattern.reset(_filepath->getValue(), false, 0, 1);
	}
	else if( paramName == kEXRWriterHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kEXRWriterHelpString );
	}
}

}
}
}
}
