#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"

#include <ImfInputFile.h>
#include <ImathBox.h>
#include <ImfChannelList.h>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {
using namespace Imf;
using namespace boost::gil;

EXRReaderPlugin::EXRReaderPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle ),
	_dstClip( 0 )
{
	_dstClip       = fetchClip( kOfxImageEffectOutputClipName );
	_filepath      = fetchStringParam( kInputFilename );
	_outComponents = fetchChoiceParam( kOutputComponents );
	_vChannelChoice.push_back( fetchChoiceParam( kOutputRedIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kOutputGreenIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kOutputBlueIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kOutputAlphaIs ) );

	std::string sFilepath;
	_filepath->getValue( sFilepath );
	if( exists( sFilepath ) )
	{
		InputFile in( sFilepath.c_str() );
		const Header& h             = in.header();
		const Imath::V2i dataWindow = h.dataWindow().size();
		_imageDims.x = dataWindow.x + 1;
		_imageDims.y = dataWindow.y + 1;
	}
	else
	{
		_imageDims.x = 0;
		_imageDims.y = 0;
	}
}

OFX::Clip* EXRReaderPlugin::getDstClip() const
{
	return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRReaderPlugin::render( const OFX::RenderArguments& args )
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
				EXRReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				EXRReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				EXRReaderProcess<rgba32f_view_t> fred( *this );
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

void EXRReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kExrReaderHelpString );
	}
	else if( paramName == kInputFilename )
	{
		std::string sFilepath;
		_filepath->getValue( sFilepath );
		// Check if exist
		if( exists( sFilepath ) )
		{
			// read dims
			InputFile in( sFilepath.c_str() );
			const Header& h             = in.header();
			const Imath::V2i dataWindow = h.dataWindow().size();
			_imageDims.x = dataWindow.x + 1;
			_imageDims.y = dataWindow.y + 1;
			const ChannelList& cl = h.channels();
			int nc                = 0;

			_outComponents->resetOptions();
			// Unhide output component type setup
			_outComponents->setIsSecret( false );
			// Hide output channel selection till we don't select a channel.
			for( size_t i = 0; i < _vChannelChoice.size(); ++i )
				_vChannelChoice[i]->setIsSecret( true );
			_vChannelNames.clear();
			for( ChannelList::ConstIterator it = cl.begin(); it != cl.end(); ++it )
			{
				_vChannelNames.push_back( it.name() );
				for( size_t j = 0; j < _vChannelChoice.size(); ++j )
					_vChannelChoice[j]->appendOption( it.name() );
				++nc;
				switch( nc )
				{
					case 1:
					{
						_outComponents->appendOption( "Gray" );
						_outComponents->setValue( 0 );
						for( size_t j = 0; j < _vChannelChoice.size(); ++j )
							_vChannelChoice[j]->setValue( 0 );
						break;
					}
					case 3:
					{
						_outComponents->appendOption( "RGB" );
						_outComponents->setValue( 1 );
						size_t nc = _vChannelChoice.size() - 1;
						for( size_t j = 0; j < nc; ++j )
							_vChannelChoice[nc - j - 1]->setValue( j );
						_vChannelChoice[3]->setValue( 0 );
						break;
					}
					case 4:
					{
						_outComponents->appendOption( "RGBA" );
						_outComponents->setValue( 2 );
						size_t nc = _vChannelChoice.size();
						for( size_t j = 0; j < nc; ++j )
							_vChannelChoice[nc - j - 1]->setValue( j );
						break;
					}
				}
			}
		}
		else
		{
			_imageDims.x = 0;
			_imageDims.y = 0;
		}
	}
	else if( paramName == kOutputComponents )
	{
		switch( _outComponents->getValue() )
		{
			case 0:
			{
				for( size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
					_vChannelChoice[j]->setIsSecret( false );
				_vChannelChoice[3]->setIsSecret( true );
				break;
			}
			case 1:
			{
				for( size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
					_vChannelChoice[j]->setIsSecret( true );
				_vChannelChoice[3]->setIsSecret( false );
				break;
			}
			case 2:
			{
				for( size_t j = 0; j < _vChannelChoice.size(); ++j )
					_vChannelChoice[j]->setIsSecret( true );
				break;
			}
		}
	}
}

void EXRReaderPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	const OfxRectD roi = { 0.0, 0.0, _imageDims.x * _dstClip->getPixelAspectRatio(), _imageDims.y };

	rois.setRegionOfInterest( *_dstClip, roi );
}

bool EXRReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.x2 = _imageDims.x * _dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = _imageDims.y;
	return true;
}

void EXRReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
	clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthFloat );
	clipPreferences.setPixelAspectRatio( *_dstClip, 720.0 / 720.0 );
}

}
}
}
}
