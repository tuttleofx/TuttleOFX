#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterProcess.hpp"
#include "Jpeg2000WriterDefinitions.hpp"

#include <openjpeg/J2KWriter.hpp>

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

using namespace tuttle::io;
using namespace boost::gil;

Jpeg2000WriterPlugin::Jpeg2000WriterPlugin( OfxImageEffectHandle handle )
: WriterPlugin( handle )
{
	_cineProfil = fetchChoiceParam( kParamCinemaProfil );
	_lossless = fetchBooleanParam( kParamLossless );
}

Jpeg2000ProcessParams Jpeg2000WriterPlugin::getProcessParams(const OfxTime time)
{
	Jpeg2000ProcessParams params;
	params._filepath = this->_filePattern.getFilenameAt(time);
	switch(static_cast<EParamBitDepth>(this->_paramBitDepth->getValue()))
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth12:
			params._bitDepth = 12;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		case eParamBitDepth32:
			params._bitDepth = 32;
			break;
		default:
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
	_cineProfil->getValue( params._cineProfil );
	_lossless->getValue( params._lossless );
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000WriterPlugin::render( const OFX::RenderArguments &args )
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
					Jpeg2000WriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					Jpeg2000WriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					Jpeg2000WriterProcess<rgba32f_view_t> fred( *this );
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
					Jpeg2000WriterProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					Jpeg2000WriterProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					Jpeg2000WriterProcess<gray32f_view_t> fred( *this );
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

void Jpeg2000WriterPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	WriterPlugin::changedParam( args, paramName );
	if( paramName == kParamCinemaProfil && args.reason == OFX::eChangeUserEdit )
	{
		if (_cineProfil->getValue() != 0)
		{
			_lossless->setEnabled(false);
			// DCI needs 12b
			_paramBitDepth->setValue(1);
			_paramBitDepth->setEnabled(false);
		}
		else
		{
			_lossless->setEnabled(true);
			_paramBitDepth->setEnabled(true);
		}
	}
	else if( paramName == "Help" )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kJ2KHelpString );
    }
}

}
}
}
}
