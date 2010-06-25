#include "WriterPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

WriterPlugin::WriterPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
{
	_srcClip        = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_dstClip        = fetchClip( kOfxImageEffectOutputClipName );
	_filepath       = fetchStringParam( kTuttlePluginWriterParamFilename );
	_renderButton   = fetchPushButtonParam( kTuttlePluginWriterParamRender );
	_renderAlways   = fetchBooleanParam( kTuttlePluginWriterParamRenderAlways );
	_bitDepth       = fetchChoiceParam( kTuttlePluginWriterParamBitDepth );
	_filePattern.reset(_filepath->getValue(), false, 0, 1);
}

WriterPlugin::~WriterPlugin()
{
}

void WriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

void WriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginWriterParamFilename )
	{
		_filePattern.reset(_filepath->getValue(), false, 0, 1);
	}
}

}
}
