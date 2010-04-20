#include "ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_dstClip  = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kTuttlePluginReaderParamFilename );
	_fPattern.reset(_filepath->getValue(), true);
	_explicitConv = fetchChoiceParam( kTuttlePluginReaderParamExplicitConversion );
}

ReaderPlugin::~ReaderPlugin() {
}

void ReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginReaderParamFilename )
	{
		_fPattern.reset(_filepath->getValue(), true);
	}
}

void ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	OfxRangeI rangei = _fPattern.getRange();
	range.min = (double)rangei.min;
	range.max = (double)rangei.max;
	return true;
}

}
}
