#include "ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	_clipDst           = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath     = fetchStringParam( kReaderParamFilename );
	_isSequence        = _filePattern.initFromDetection( _paramFilepath->getValue() );
	_paramExplicitConv = fetchChoiceParam( kReaderParamExplicitConversion );
}

ReaderPlugin::~ReaderPlugin()
{}

void ReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kReaderParamFilename )
	{
		_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
	}
}

void ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	const std::string filename( getAbsoluteFirstFilename() );
	if( !bfs::exists( filename ) )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist( filename ) );
	}
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	range.min = getFirstTime();
	range.max = getLastTime();
	return true;
}

void ReaderPlugin::render( const OFX::RenderArguments& args )
{
	const std::string filename( getAbsoluteFilenameAt( args.time ) );
	if( ! boost::filesystem::exists( filename ) )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist( filename ) );
	}
	
}

}
}
