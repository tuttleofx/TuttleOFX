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

	const OFX::EBitDepth eOfxBitDepth = _clipDst->getPixelDepth( );
	const OFX::EPixelComponent components = _clipDst->getPixelComponents( );
	
	int pixelBytes = 0;
	
	switch( eOfxBitDepth )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch( )
								 << exception::user( "Writer: Unable to compute custom or non bit depth" ) );
			break;
		case OFX::eBitDepthUByte: pixelBytes = 1;
			break;
		case OFX::eBitDepthUShort: pixelBytes = 2;
			break;
		case OFX::eBitDepthFloat: pixelBytes = 4;
			break;
	}
	
	switch( components )
	{
		case OFX::ePixelComponentAlpha: break; // pixelSize * 1
		case OFX::ePixelComponentRGB: pixelBytes *= 3;
			break;
		case OFX::ePixelComponentRGBA: pixelBytes *= 4;
			break;
		default: break;
	}

	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );
	
	const OfxRectI rod = _clipDst->getPixelRod( args.time, args.renderScale );
	const OfxPointI rodSize = _clipDst->getPixelRodSize( args.time, args.renderScale );
	const std::size_t rowBytesToCopy = rodSize.x * pixelBytes;
	for( int y = rod.y1; y < rod.y2; ++y )
	{
		void* dataSrcPtr = src->getPixelAddress( rod.x1, y );
		void* dataDstPtr = dst->getPixelAddress( rod.x1, y );
		memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
	}
}

}
}
