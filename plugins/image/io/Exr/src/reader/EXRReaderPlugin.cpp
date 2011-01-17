#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/plugin/context/ReaderPlugin.hpp>
#include <tuttle/plugin/context/WriterDefinition.hpp>

#include <ImfInputFile.h>
#include <ImathBox.h>
#include <ImfChannelList.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

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

EXRReaderProcessParams EXRReaderPlugin::getProcessParams( const OfxTime time )
{
	EXRReaderProcessParams params;

	params._filepath      = getAbsoluteFilenameAt( time );
	params._outComponents = _outComponents->getValue();
	params._flip          = _paramFlip->getValue();

	return params;
}

void EXRReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamReaderFilename )
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
				for( std::size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( false );
				}
				_vChannelChoice[3]->setIsSecret( true );
				break;
			}
			case 1:
			{
				for( std::size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( true );
				}
				_vChannelChoice[3]->setIsSecret( false );
				break;
			}
			case 2:
			{
				for( std::size_t j = 0; j < _vChannelChoice.size(); ++j )
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
void EXRReaderPlugin::updateCombos()
{
	const std::string filename( getAbsoluteFirstFilename() );

	if( bfs::exists( filename ) )
	{
		// read dims
		InputFile in( filename.c_str() );
		const Header& h       = in.header();
		const ChannelList& cl = h.channels();
		int nc                = 0;

		_outComponents->resetOptions();
		// Unhide output component type setup
		_outComponents->setIsSecret( false );
		// Hide output channel selection till we don't select a channel.
		for( std::size_t i = 0; i < _vChannelChoice.size(); ++i )
		{
			_vChannelChoice[i]->setIsSecret( true );
		}
		_vChannelNames.clear();
		for( ChannelList::ConstIterator it = cl.begin(); it != cl.end(); ++it )
		{
			_vChannelNames.push_back( it.name() );
			for( std::size_t j = 0; j < _vChannelChoice.size(); ++j )
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
					for( std::size_t j = 0; j < _vChannelChoice.size(); ++j )
						_vChannelChoice[j]->setValue( 0 );
					break;
				}
				case 3:
				{
					_outComponents->appendOption( "RGB" );
					_outComponents->setValue( 1 );
					size_t nc = _vChannelChoice.size() - 1;
					for( std::size_t j = 0; j < nc; ++j )
						_vChannelChoice[nc - j - 1]->setValue( j );
					_vChannelChoice[3]->setValue( 0 );
					break;
				}
				case 4:
				{
					_outComponents->appendOption( "RGBA" );
					_outComponents->setValue( 2 );
					size_t nc = _vChannelChoice.size();
					for( std::size_t j = 0; j < nc; ++j )
						_vChannelChoice[nc - j - 1]->setValue( j );
					break;
				}
			}
		}
	}
}

void EXRReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename( getAbsoluteFirstFilename() );
	switch( getExplicitConversion() )
	{
		case eParamReaderExplicitConversionAuto:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat ); /// @todo tuttle: retrieve info from exr, can be 8, 16, 32 as interger or floating point values
			break;
		}
		case eParamReaderExplicitConversionByte:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
			break;
		}
		case eParamReaderExplicitConversionShort:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
			break;
		}
		case eParamReaderExplicitConversionFloat:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			break;
		}
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 ); /// @todo tuttle: retrieve info from exr
}

bool EXRReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	InputFile in( getAbsoluteFilenameAt( args.time ).c_str() );
	const Header& h             = in.header();
	const Imath::V2i dataWindow = h.dataWindow().size();

	rod.x1 = 0;
	rod.x2 = ( dataWindow.x + 1 ) * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dataWindow.y + 1;
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );
	doGilRender<EXRReaderProcess>( *this, args );
}


}
}
}
}
