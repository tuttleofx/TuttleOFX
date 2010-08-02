#include "PngWriterDefinitions.hpp"
#include "PngWriterPlugin.hpp"
#include "PngWriterProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

PngWriterPlugin::PngWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_outputRGB     = fetchBooleanParam( kParamOutputRGB );
}

PngWriterProcessParams PngWriterPlugin::getProcessParams(const OfxTime time)
{
	PngWriterProcessParams params;
	params._filepath = this->_filePattern.getFilenameAt(time);
	params._outputRGB = this->_outputRGB->getValue();
	switch(static_cast<EParamBitDepth>(this->_paramBitDepth->getValue()))
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		default:
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PngWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _paramRenderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _clipSrc->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _clipSrc->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					PngWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					PngWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					PngWriterProcess<rgba32f_view_t> fred( *this );
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
					PngWriterProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					PngWriterProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					PngWriterProcess<gray32f_view_t> fred( *this );
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
