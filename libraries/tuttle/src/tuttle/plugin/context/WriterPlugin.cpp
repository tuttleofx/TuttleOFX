#include "WriterPlugin.hpp"

#include <ofxCore.h>

#include <cstdio>

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

WriterPlugin::WriterPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
, _oneRender( false )
, _oneRenderAtTime( 0 )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginFilename );
	_paramRenderButton = fetchPushButtonParam( kParamWriterRender );
	_paramRenderAlways = fetchBooleanParam( kParamWriterRenderAlways );
	_paramBitDepth = fetchChoiceParam( kTuttlePluginBitDepth );
	_paramPremult = fetchBooleanParam( kParamPremultiplied );
	_paramForceNewRender = fetchIntParam( kParamWriterForceNewRender );
	_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue( ) );
}

WriterPlugin::~WriterPlugin( )
{
}

void WriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginFilename )
	{
		_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue( ) );
	}
	else if( paramName == kParamWriterRender )
	{
		_oneRender = true;
		_oneRenderAtTime = args.time;
		// modify the plugin to force a new render
		_paramForceNewRender->setValue( _paramForceNewRender->getValue( ) + 1 );
	}
}

void WriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( varyOnTime( ) );
}

bool WriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	// little hack for the push button Render
	if( _oneRender && _oneRenderAtTime == args.time )
	{
		return false;
	}
	if( OFX::getImageEffectHostDescription( )->hostIsBackground )
	{
		return false;
	}
	if( _paramRenderAlways->getValue( ) )
	{
		return false;
	}
	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

void WriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	boost::filesystem::path dir( getAbsoluteDirectory( ) );
	if( !boost::filesystem::exists( dir ) )
	{
		boost::filesystem::create_directories( dir );
	}
}

void WriterPlugin::render( const OFX::RenderArguments& args )
{
	_oneRender = false;

	TUTTLE_TCOUT( "        --> " << getAbsoluteFilenameAt( args.time ) );

	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );
	
	// Copy buffer
	const OfxRectI bounds = dst->getBounds();
	TUTTLE_TCOUT_VAR( bounds );
	if( src->isLinearBuffer() && dst->isLinearBuffer() )
	{
		TUTTLE_TCOUT( "isLinearBuffer" );
		const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
		TUTTLE_TCOUT_VAR( imageDataBytes );
		if( imageDataBytes )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, bounds.y1 );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, bounds.y1 );
			memcpy( dataDstPtr, dataSrcPtr, imageDataBytes );
		}
	}
	else
	{
		const std::size_t rowBytesToCopy = dst->getBoundsRowDataBytes();
		for( int y = bounds.y1; y < bounds.y2; ++y )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, y );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, y );
			memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
		}
	}
}

}
}
