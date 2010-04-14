#include "PNGWriterDefinitions.hpp"
#include "PNGWriterPlugin.hpp"
#include "PNGWriterProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

PNGWriterPlugin::PNGWriterPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_srcClip        = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip        = fetchClip( kOfxImageEffectOutputClipName );
	_filepath       = fetchStringParam( kOutputFilename );
	_renderButton   = fetchPushButtonParam( kRender );
	_renderAlways   = fetchBooleanParam( kParamRenderAlways );
	_precision      = fetchChoiceParam( kPrecision );
	_precisionLong  = fetchChoiceParam( kPrecisionLong );
}

OFX::Clip* PNGWriterPlugin::getSrcClip() const
{
	return _srcClip;
}

OFX::Clip* PNGWriterPlugin::getDstClip() const
{
	return _dstClip;
}

PNGWriterParams PNGWriterPlugin::getParams(const OfxTime time)
{
	PNGWriterParams params;
	params._filepath = _fPattern.getFilenameAt(time);
	params._precision = _precision->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PNGWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _renderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
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
		else if( dstComponents == OFX::ePixelComponentCustom )
		{
			COUT_FATALERROR( "Custom pixel component is not recognize !" );
		}
		else if( dstComponents == OFX::ePixelComponentNone )
		{
			COUT_FATALERROR( "Pixel component is None !" );
		}
	}
}

void PNGWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kOutputFilename )
	{
		_fPattern.reset(_filepath->getValue(), false, 0, 1);
	}
	else if( paramName == kPrecisionLong && args.reason == OFX::eChangeUserEdit  )
	{
		_precision->setValue( _precisionLong->getValue() );
	}
	else if( paramName == kPrecision && args.reason == OFX::eChangeUserEdit )
	{
		_precisionLong->setValue( _precision->getValue() );
	}
}

void PNGWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
	switch(_precision->getValue())
	{
		case 8:
			clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
			break;
		case 12:
		case 16:
			clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUShort );
			break;
		case 32:
			clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthFloat );
			break;
	}
}

}
}
}
}
