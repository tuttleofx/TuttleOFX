#include "ViewerPlugin.hpp"
#include "ViewerProcess.hpp"
#include "ViewerDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace viewer {


ViewerPlugin::ViewerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );
}

ViewerProcessParams<ViewerPlugin::Scalar> ViewerPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ViewerProcessParams<Scalar> params;
	return params;
}

void ViewerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

bool ViewerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ViewerPlugin::render( const OFX::RenderArguments &args )
{
	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );

	// Copy buffer
	const OfxRectI bounds = dst->getBounds();
	TUTTLE_LOG_VAR( TUTTLE_INFO, bounds );

	size_t width = dst->getBoundsSize().x;
	size_t height = dst->getBoundsSize().y;
	size_t components = 0;

	GLenum format = GL_RGB;
	GLenum type = GL_FLOAT;

	switch( dst->getPixelDepth() )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch()
				<< exception::user( "Viewer: Unable to compute custom or non bit depth" ) );
			break;
		case OFX::eBitDepthUByte:  type = GL_UNSIGNED_BYTE; break;
		case OFX::eBitDepthUShort: type = GL_UNSIGNED_SHORT; break;
		case OFX::eBitDepthFloat:  type = GL_FLOAT; break;
	}
	switch( dst->getPixelComponents() )
	{
		case OFX::ePixelComponentAlpha: components = 1; format = GL_LUMINANCE; break;
		case OFX::ePixelComponentRGB  : components = 3; format = GL_RGB; break;
		case OFX::ePixelComponentRGBA : components = 4; format = GL_RGBA; break;
		default:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch()
				<< exception::user( "Viewer: Unable to compute unknown component." ) );
			break;
	}

	size_t imgSizeBytes = width * height * dst->getPixelBytes() ;

	char* data = new char[ imgSizeBytes ];
	char* tmpData = data;

	if( src->isLinearBuffer() && dst->isLinearBuffer() )
	{
		TUTTLE_LOG_INFO( "isLinearBuffer" );
		const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
		TUTTLE_LOG_VAR( TUTTLE_INFO, imageDataBytes );
		if( imageDataBytes )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, bounds.y1 );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, bounds.y1 );
			memcpy( dataDstPtr, dataSrcPtr, imageDataBytes );
			memcpy( tmpData, dataSrcPtr, imageDataBytes );
			tmpData += imageDataBytes;
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
			memcpy( tmpData, dataSrcPtr, rowBytesToCopy );
			tmpData += rowBytesToCopy;
		}
	}

	openGLWindow( width, height );

	loadNewTexture( data, components, width, height, format, type );

	glutIdleFunc(idle);
	glutMainLoop();
}


}
}
}
