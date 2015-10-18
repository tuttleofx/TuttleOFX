#include "ReaderPlugin.hpp"

#include <filesystem.hpp>

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
	, _isSequence( false )
	, _filePattern()
{
	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginFilename );
	_isSequence    = sequenceParser::browseSequence( _filePattern, _paramFilepath->getValue() );
	_paramBitDepth = fetchChoiceParam( kTuttlePluginBitDepth );
	_paramChannel  = fetchChoiceParam( kTuttlePluginChannel );
}

ReaderPlugin::~ReaderPlugin()
{}

void ReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginFilename )
	{
		_isSequence = sequenceParser::browseSequence( _filePattern, _paramFilepath->getValue() );
	}
}

void ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );

	switch( getExplicitBitDepthConversion() )
	{
		case eParamReaderBitDepthByte:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
			break;
		}
		case eParamReaderBitDepthShort:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
			break;
		}
		case eParamReaderBitDepthAuto:
		case eParamReaderBitDepthFloat:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			break;
		}
	}
	switch( getExplicitChannelConversion() )
	{
		case eParamReaderChannelGray:
		{
			clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentAlpha );
			break;
		}
		case eParamReaderChannelRGB:
		{
			if( OFX::getImageEffectHostDescription()->supportsPixelComponent( OFX::ePixelComponentRGB ) )
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGB );
			else
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
			break;
		}
		case eParamReaderChannelAuto:
		case eParamReaderChannelRGBA:
		{
			clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
			break;
		}
	}

	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	range.min = getFirstTime();
	range.max = getLastTime();
	TUTTLE_LOG_INFO( "[Reader plugin] Time Domain : " << range.min << " to " << range.max );
	return true;
}

void ReaderPlugin::render( const OFX::RenderArguments& args )
{
	std::string filename =  getAbsoluteFilenameAt( args.time );
	TUTTLE_LOG_INFO( "        >-- " << filename );
}

std::string ReaderPlugin::getAbsoluteFilenameAt( const OfxTime time ) const
{
	if( _isSequence )
	{
		bfs::path dir( getAbsoluteDirectory() );
		bfs::path filename( _filePattern.getFilenameAt( boost::numeric_cast<sequenceParser::Time>(time) ) );
		return (dir / filename).string();
	}
	else
	{
		bfs::path filepath( _paramFilepath->getValue( ) );
		return bfs::absolute( filepath ).string( );
	}
}

std::string ReaderPlugin::getAbsoluteFirstFilename() const
{
	if( _isSequence )
	{
		bfs::path dir( getAbsoluteDirectory() );
		bfs::path filename( _filePattern.getFirstFilename() );
		return (dir / filename).string();
	}
	else
	{
		bfs::path filepath( _paramFilepath->getValue() );
		return bfs::absolute(filepath).string();
	}
}

std::string ReaderPlugin::getAbsoluteDirectory() const 
{
	bfs::path filepath( _paramFilepath->getValue() );
	return bfs::absolute(filepath).parent_path().string();
}

OfxTime ReaderPlugin::getFirstTime() const
{
	if( _isSequence )
		return _filePattern.getFirstTime();
	else
		return kOfxFlagInfiniteMin;
}

OfxTime ReaderPlugin::getLastTime() const
{
	if( _isSequence )
		return _filePattern.getLastTime();
	else
		return kOfxFlagInfiniteMax;
}

EParamReaderBitDepth ReaderPlugin::getExplicitBitDepthConversion() const
{
	return static_cast<EParamReaderBitDepth>( _paramBitDepth->getValue() );
}


EParamReaderChannel ReaderPlugin::getExplicitChannelConversion() const
{
	return static_cast<EParamReaderChannel>( _paramChannel->getValue() );
}

OFX::EBitDepth ReaderPlugin::getOfxExplicitConversion() const
{
	switch( getExplicitBitDepthConversion() )
	{
		case eParamReaderBitDepthByte:
			return OFX::eBitDepthUByte;
		case eParamReaderBitDepthShort:
			return OFX::eBitDepthUShort;
		case eParamReaderBitDepthFloat:
			return OFX::eBitDepthFloat;
		case eParamReaderBitDepthAuto:
			BOOST_THROW_EXCEPTION( exception::Value() );
	}
	return OFX::eBitDepthNone;
}

}
}
