#include "ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	COUT( "ReaderPlugin begin" );
	_clipDst           = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath     = fetchStringParam( kReaderParamFilename );
	_isSequence        = _filePattern.initFromDetection( _paramFilepath->getValue() );
	_paramExplicitConv = fetchChoiceParam( kReaderParamExplicitConversion );
	COUT( "ReaderPlugin end" );
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
	COUT( "ReaderPlugin::getClipPreferences" );
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	COUT( "ReaderPlugin::getTimeDomain" );
	range.min = getFirstTime();
	range.max = getLastTime();
	COUT_VAR( range );
	return true;
}

}
}
