#include "WriterPlugin.hpp"

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

WriterPlugin::WriterPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
, _oneRender(false)
, _oneRenderAtTime(0)
{
	_clipSrc        = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst        = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath       = fetchStringParam( kTuttlePluginWriterParamFilename );
	_paramRenderButton   = fetchPushButtonParam( kTuttlePluginWriterParamRender );
	_paramRenderAlways   = fetchBooleanParam( kTuttlePluginWriterParamRenderAlways );
	_paramBitDepth       = fetchChoiceParam( kTuttlePluginWriterParamBitDepth );
	_paramForceNewRender = fetchIntParam( kTuttlePluginWriterParamForceNewRender );
	_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
}

WriterPlugin::~WriterPlugin()
{
}

void WriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginWriterParamFilename )
	{
		_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
	}
	else if( paramName == kTuttlePluginWriterParamRender )
	{
		_oneRender = true;
		_oneRenderAtTime = args.time;
		// modify the plugin to force a new render
		_paramForceNewRender->setValue( _paramForceNewRender->getValue() + 1 );
	}
}

void WriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime() );
}

bool WriterPlugin::isIdentity( const OFX::RenderArguments &args, OFX::Clip * &identityClip, OfxTime &identityTime )
{
	// little hack for the push button Render
	if( _oneRender && _oneRenderAtTime == args.time )
	{
		return false;
	}
	if( OFX::getImageEffectHostDescription()->hostIsBackground )
	{
		return false;
	}
	if( _paramRenderAlways->getValue() )
	{
		return false;
	}
	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

void WriterPlugin::render( const OFX::RenderArguments &args )
{
	_oneRender = false;
}

}
}
