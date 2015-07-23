#include "WriterPlugin.hpp"

#include <ofxCore.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/scoped_ptr.hpp>

#include <filesystem.hpp>
#include <cstdio>

namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

WriterPlugin::WriterPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
	, _oneRender( false )
	, _oneRenderAtTime( 0 )
	, _isSequence( false )
	, _filePattern()
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginFilename );
	_paramRenderButton = fetchPushButtonParam( kParamWriterRender );
	_paramRenderAlways = fetchBooleanParam( kParamWriterRenderAlways );
	_paramCopyToOutput = fetchBooleanParam( kParamWriterCopyToOutput );
	_paramBitDepth = fetchChoiceParam( kTuttlePluginBitDepth );
	_paramPremult = fetchBooleanParam( kParamPremultiplied );
	_paramExistingFile = fetchChoiceParam( kParamWriterExistingFile );
	_paramForceNewRender = fetchIntParam( kParamWriterForceNewRender );
}

WriterPlugin::~WriterPlugin( )
{}

void WriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginFilename )
	{
		bfs::path path( _paramFilepath->getValue() );
		_isSequence = sequenceParser::browseSequence( _filePattern, path.filename().string() );
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
	EParamWriterExistingFile existingFile = static_cast<EParamWriterExistingFile>(_paramExistingFile->getValue());
	if( existingFile != eParamWriterExistingFile_overwrite )
	{
		const std::string filepath = getAbsoluteFilenameAt( args.time );
		const bool fileExists = bfs::exists( filepath );

		switch( existingFile )
		{
			case eParamWriterExistingFile_error:
			{
				if( fileExists )
					BOOST_THROW_EXCEPTION( exception::FileExist(filepath) );
				break;
			}
			case eParamWriterExistingFile_reader:
			{
				BOOST_ASSERT(false);
				// Not implemented
			}
			case eParamWriterExistingFile_skip:
			{
				if( fileExists )
				{
					// We declare an empty clip as identity to disable the process of this node.
					// This is not in the OpenFX standard. So this option only exist on TuttleOFX host.
					identityClip = NULL;
					identityTime = 0;
					TUTTLE_TLOG_TRACE("[Plugin Writer] Identity node: " << this->getName() << " at time: " << args.time  << ", file already exist:" << filepath);
					return true;
				}
				break;
			}
			case eParamWriterExistingFile_overwrite:
				BOOST_ASSERT(false);
		}
	}
	
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
	bfs::path dir( getAbsoluteDirectory() );
	if( !bfs::exists( dir ) )
	{
		bfs::create_directories( dir );
	}
}

void WriterPlugin::render( const OFX::RenderArguments& args )
{
	_oneRender = false;

	TUTTLE_LOG_INFO( "        --> " << getAbsoluteFilenameAt( args.time ) );

	if( _paramCopyToOutput->getValue() )
	{
		boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
		boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );

		// Copy buffer
		const OfxRectI bounds = dst->getBounds();
		TUTTLE_TLOG_VAR( TUTTLE_TRACE, bounds );
		if( src->isLinearBuffer() && dst->isLinearBuffer() )
		{
			TUTTLE_TLOG( TUTTLE_TRACE, "isLinearBuffer" );
			const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
			TUTTLE_TLOG_VAR( TUTTLE_TRACE, imageDataBytes );
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

std::string WriterPlugin::getAbsoluteFilenameAt( const OfxTime time ) const
{
	if( _isSequence )
	{
		bfs::path dir( getAbsoluteDirectory() );
		bfs::path filename( _filePattern.getFilenameAt( boost::numeric_cast<sequenceParser::Time>(time) ) );
		return (dir / filename).string();
	}
	else
	{
		bfs::path filepath( _paramFilepath->getValue( ) );
		return bfs::absolute( filepath ).string( );
	}
}

std::string WriterPlugin::getAbsoluteFirstFilename() const
{
	if( _isSequence )
	{
		bfs::path dir( getAbsoluteDirectory() );
		bfs::path filename (_filePattern.getFirstFilename() );
		return (dir / filename).string();
	}
	else
	{
		bfs::path filepath( _paramFilepath->getValue( ) );
		return bfs::absolute( filepath ).string( );
	}
}

std::string WriterPlugin::getAbsoluteDirectory() const
{
	bfs::path filepath( _paramFilepath->getValue() );
	return bfs::absolute(filepath).parent_path().string();
}

OfxTime WriterPlugin::getFirstTime() const
{
	if( _isSequence )
		return _filePattern.getFirstTime();
	else
		return kOfxFlagInfiniteMin;
}

OfxTime WriterPlugin::getLastTime() const
{
	if( _isSequence )
		return _filePattern.getLastTime();
	else
		return kOfxFlagInfiniteMax;
}

}
}
