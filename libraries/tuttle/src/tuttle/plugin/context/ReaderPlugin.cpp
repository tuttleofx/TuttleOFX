#include "ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipDst  = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginReaderParamFilename );
	_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
	_paramExplicitConv = fetchChoiceParam( kTuttlePluginReaderParamExplicitConversion );
}

ReaderPlugin::~ReaderPlugin()
{
}

void ReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginReaderParamFilename )
	{
		_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
	}
}

void ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	range.min = getFirstTime();
	range.max = getLastTime();
	return true;
}

}
}
