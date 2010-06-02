#include "ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

ReaderPlugin::ReaderPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipDst  = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kTuttlePluginReaderParamFilename );
	_filePattern.reset(_filepath->getValue(), true);
	_explicitConv = fetchChoiceParam( kTuttlePluginReaderParamExplicitConversion );
}

ReaderPlugin::~ReaderPlugin()
 {
}

void ReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginReaderParamFilename )
	{
		_filePattern.reset(_filepath->getValue(), true);
	}
}

void ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool ReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	if (varyOnTime())
	{
		OfxRangeI rangei = _filePattern.getRange();
		range.min = (double)rangei.min;
		range.max = (double)rangei.max;
	}
	else
	{
		range.min = (double)kOfxFlagInfiniteMin;
		range.max = (double)kOfxFlagInfiniteMax;
	}
	return true;
}

}
}
