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
	: WriterPlugin( handle )
{
	_outputRGB     = fetchBooleanParam( kParamOutputRGB );
}

PNGWriterProcessParams PNGWriterPlugin::getParams(const OfxTime time)
{
	PNGWriterProcessParams params;
	params._filepath = this->_fPattern.getFilenameAt(time);
	params._outputRGB = this->_outputRGB->getValue();
	switch(static_cast<EParamBitDepth>(this->_bitDepth->getValue()))
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		default:
			throw( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
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
		else
		{
			COUT_FATALERROR( "Pixel component unrecognize ! (" << mapPixelComponentEnumToStr( dstComponents ) << ")" );
		}
	}
}

}
}
}
}
