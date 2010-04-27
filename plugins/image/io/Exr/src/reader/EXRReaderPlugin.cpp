#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"
#include "tuttle/plugin/FilenameManager.hpp"
#include "tuttle/plugin/context/ReaderPlugin.hpp"
#include "tuttle/plugin/context/WriterDefinition.hpp"

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

namespace bfs = boost::filesystem;
using namespace Imf;
using namespace boost::gil;

EXRReaderPlugin::EXRReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
	_outComponents = fetchChoiceParam( kParamOutputComponents );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputRedIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputGreenIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputBlueIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputAlphaIs ) );
	updateCombos();
}

EXRReaderProcessParams EXRReaderPlugin::getProcessParams(const OfxTime time)
{
	EXRReaderProcessParams params;
	params._filepath = _filePattern.getFilenameAt(time);
	params._outComponents = _outComponents->getValue();
	return params;
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

bool EXRReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	InputFile in( this->_filePattern.getFilenameAt(args.time).c_str() );
	const Header& h             = in.header();
	const Imath::V2i dataWindow = h.dataWindow().size();
	rod.x1 = 0;
	rod.x2 = (dataWindow.x + 1) * this->_dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dataWindow.y + 1;
	return true;
}

void EXRReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kExrReaderHelpString );
	}
	else if( paramName == kTuttlePluginReaderParamFilename )
	{
		ReaderPlugin::changedParam( args, paramName );
		updateCombos();
	}
	else if( paramName == kParamOutputComponents )
	{
		switch( _outComponents->getValue() )
		{
			case 0:
			{
				for( size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( false );
				}
				_vChannelChoice[3]->setIsSecret( true );
				break;
			}
			case 1:
			{
				for( size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( true );
				}
				_vChannelChoice[3]->setIsSecret( false );
				break;
			}
			case 2:
			{
				for( size_t j = 0; j < _vChannelChoice.size(); ++j )
				{
					_vChannelChoice[j]->setIsSecret( true );
				}
				break;
			}
		}
	}
	else
	{
		ReaderPlugin::changedParam( args, paramName );
	}
}

void EXRReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	// Check if exist
	if( bfs::exists( _filePattern.getFirstFilename() ) )
	{
		if ( _explicitConv->getValue() )
		{
			switch( _explicitConv->getValue() )
			{
				case 1:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUByte );
					break;
				}
				case 2:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUShort );
					break;
				}
				case 3:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthFloat );
					break;
				}
			}
		}
		else
		{
			clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthFloat );
		}
		clipPreferences.setClipComponents( *this->_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setPixelAspectRatio( *this->_dstClip, 1.0 );
	}
}

void EXRReaderPlugin::updateCombos()
{
	if ( bfs::exists( this->_filePattern.getFirstFilename() ) )
	{
		// read dims
		InputFile in( this->_filePattern.getFirstFilename().c_str() );
		const Header& h       = in.header();
		const ChannelList& cl = h.channels();
		int nc                = 0;

		_outComponents->resetOptions();
		// Unhide output component type setup
		_outComponents->setIsSecret( false );
		// Hide output channel selection till we don't select a channel.
		for( size_t i = 0; i < _vChannelChoice.size(); ++i )
		{
			_vChannelChoice[i]->setIsSecret( true );
		}
		_vChannelNames.clear();
		for( ChannelList::ConstIterator it = cl.begin(); it != cl.end(); ++it )
		{
			_vChannelNames.push_back( it.name() );
			for( size_t j = 0; j < _vChannelChoice.size(); ++j )
			{
				_vChannelChoice[j]->appendOption( it.name() );
			}
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
}

}
}
}
}
