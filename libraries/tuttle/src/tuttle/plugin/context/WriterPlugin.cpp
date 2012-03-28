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
    _clipSrc             = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst             = fetchClip( kOfxImageEffectOutputClipName );
    _paramFilepath       = fetchStringParam( kTuttlePluginFilename );
    _paramRenderButton   = fetchPushButtonParam( kParamWriterRender );
    _paramRenderAlways   = fetchBooleanParam( kParamWriterRenderAlways );
    _paramBitDepth       = fetchChoiceParam( kTuttlePluginBitDepth );
    _paramForceNewRender = fetchIntParam( kParamWriterForceNewRender );
    _isSequence          = _filePattern.initFromDetection( _paramFilepath->getValue() );
}

WriterPlugin::~WriterPlugin()
{
}

void WriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
    if( paramName == kTuttlePluginFilename )
    {
	_isSequence = _filePattern.initFromDetection( _paramFilepath->getValue() );
    }
    else if( paramName == kParamWriterRender )
    {
	_oneRender       = true;
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

bool WriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
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

void WriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
    boost::filesystem::path dir( getAbsoluteDirectory() );
    if( ! boost::filesystem::exists( dir ) )
    {
	boost::filesystem::create_directories( dir );
    }
}

void WriterPlugin::render( const OFX::RenderArguments& args )
{
	_oneRender = false;
	void* dataSrcPtr;
	void* dataDstPtr;

	OfxPointI size = _clipSrc->getPixelRodSize( args.time );

	int pixelSize = 0;

	std::string filename =  getAbsoluteFilenameAt( args.time );
	std::cout << "        --> " << filename;

	OFX::EBitDepth        eOfxBitDepth = _clipSrc->getPixelDepth();
	OFX::EPixelComponent  components   = _clipDst->getPixelComponents();

	switch ( eOfxBitDepth )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch()
				<< exception::user( "Writer: Unable to compute custom or non bit depth" ) );
			break;
		case OFX::eBitDepthUByte:  pixelSize = 1; break;
		case OFX::eBitDepthUShort: pixelSize = 2; break;
		case OFX::eBitDepthFloat:  pixelSize = 4; break;
	}

	switch( components )
	{
		case OFX::ePixelComponentAlpha: break; // pixelSize * 1
		case OFX::ePixelComponentRGB  : pixelSize *= 3; break;
		case OFX::ePixelComponentRGBA : pixelSize *= 4; break;
		default: break;
	}

	for( int y=0; y< size.y; y++ )
	{
		dataSrcPtr = _clipSrc->fetchImage( args.time )->getPixelAddress( 0, y );
		dataDstPtr = _clipDst->fetchImage( args.time )->getPixelAddress( 0, y );

		memcpy( dataDstPtr, dataSrcPtr, size.x * pixelSize );
	}
}

}
}
