#include "RawReaderPlugin.hpp"
#include "RawReaderProcess.hpp"
#include "RawReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <libraw/libraw.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

RawReaderPlugin::RawReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
{}

RawReaderProcessParams RawReaderPlugin::getProcessParams( const OfxTime time )
{
	RawReaderProcessParams params;

	params._filepath = _filePattern.getFilenameAt( time );
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void RawReaderPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = this->_dstClip->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = this->_dstClip->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				RawReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				RawReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				RawReaderProcess<rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				RawReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				RawReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				RawReaderProcess<gray32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else
	{
		COUT_FATALERROR( dstComponents << " not recognize." );
	}
}

/*
void RawReaderPlugin::updateInfos()
{
	LibRaw _rawProcessor;
	_p1 = _rawProcessor.imgdata.idata;
	_size = _rawProcessor.imgdata.sizes;
	_color = _rawProcessor.imgdata.color;
	_thumbnail = _rawProcessor.imgdata.thumbnail;
	_p2 = _rawProcessor.imgdata.other;
	_out = _rawProcessor.imgdata.params;

	if( ( ret =  _rawProcessor.adjust_sizes_info_only() ) )
	{
		printf( "Cannot decode %s: %s\n", av[i], libraw_strerror( ret ) );
		continue;         // no recycle, open_file will recycle
	}

	printf( "\nFilename: %s\n", av[i] );
	printf( "Timestamp: %s", ctime( &( P2.timestamp ) ) );
	printf( "Camera: %s %s\n", P1.make, P1.model );
	if( P2.artist[0] )
		printf( "Owner: %s\n", P2.artist );
	if( P1.dng_version )
	{
		printf( "DNG Version: " );
		for( int i = 24; i >= 0; i -= 8 )
			printf( "%d%c", P1.dng_version >> i & 255, i ? '.' : '\n' );
	}
	printf( "ISO speed: %d\n", (int) P2.iso_speed );
	printf( "Shutter: " );
	if( P2.shutter > 0 && P2.shutter < 1 )
		P2.shutter = ( printf( "1/" ), 1 / P2.shutter );
	printf( "%0.1f sec\n", P2.shutter );
	printf( "Aperture: f/%0.1f\n", P2.aperture );
	printf( "Focal length: %0.1f mm\n", P2.focal_len );
	if( C.profile )
		printf( "Embedded ICC profile: yes, %d bytes\n", C.profile_length );
	else
		printf( "Embedded ICC profile: no\n" );

	printf( "Number of raw images: %d\n", P1.raw_count );
	if( S.pixel_aspect != 1 )
		printf( "Pixel Aspect Ratio: %0.6f\n", S.pixel_aspect );
	if( T.tlength )
		printf( "Thumb size:  %4d x %d\n", T.twidth, T.theight );
	printf( "Full size:   %4d x %d\n", S.raw_width, S.raw_height );

	printf( "Image size:  %4d x %d\n", S.width, S.height );
	printf( "Output size: %4d x %d\n", S.iwidth, S.iheight );
	printf( "Raw colors: %d", P1.colors );
	if( P1.filters )
	{
		printf( "\nFilter pattern: " );
		if( !P1.cdesc[3] )
			P1.cdesc[3] = 'G';
		for( int i = 0; i < 16; i++ )
			putchar( P1.cdesc[_rawProcessor.fc( i >> 1, i & 1 )] );
	}
	printf( "\nDaylight multipliers:" );
	for( int c = 0; c < P1.colors; c++ )
		printf( " %f", C.pre_mul[c] );
	if( C.cam_mul[0] > 0 )
	{
		printf( "\nCamera multipliers:" );
		for( int c = 0; c < 4; c++ )
			printf( " %f", C.cam_mul[c] );
	}
	const char* csl[] = { "U", "I", "CO", "L", "CA" };
	printf( "\nColor sources /Legend: (U)nknown, (I)nit, (CO)nstant, (L)oaded, (CA)lculated/:\n" );
	printf( "\tcurve=%s; rgb_cam=%s; cmatrix=%s, pre_mul=%s, cam_mul=%s",
	        csl[C.color_flags.curve_state], csl[C.color_flags.rgb_cam_state],
	        csl[C.color_flags.cmatrix_state], csl[C.color_flags.pre_mul_state],
	        csl[C.color_flags.cam_mul_state] );
	putchar( '\n' );
	printf( "Cam->XYZ matrix:\n" );
	for( int i = 0; i < 4; i++ )
		printf( "%6.4f\t%6.4f\t%6.4f\n", C.cam_xyz[i][0], C.cam_xyz[i][1], C.cam_xyz[i][2] );
}

else
{
	if( print_unpack )
	{
		char frame[32] = "";
		if( print_frame )
			snprintf( frame, 32, "%dx%dx%dx%d", S.left_margin, S.top_margin, S.right_margin, S.bottom_margin );
		printf( "%s\t%s\t%s\t%s/%s\n",
		        av[i],
		        MyCoolRawProcessor.unpack_function_name(),
		        frame,
		        P1.make, P1.model );
	}
	else
		printf( "%s is a %s %s image.\n", av[i], P1.make, P1.model );
}
MyCoolRawProcessor.recycle();
}    // endfor
}
*/

void RawReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kRawReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kRawReaderHelpString );
	}
//	else if( )
//	{}
	else
	{
		ReaderPlugin::changedParam( args, paramName );
	}
}

bool RawReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	point2<ptrdiff_t> pngDims;// = raw_read_dimensions( _filePattern.getFilenameAt( args.time ) );
	rod.x1 = 0;
	rod.x2 = pngDims.x * this->_dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = pngDims.y;
	return true;
}

void RawReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	// Check if exist
	if( bfs::exists( _filePattern.getFirstFilename() ) )
	{
		if( _explicitConv->getValue() )
		{
			switch( _explicitConv->getValue() )
			{
				case 1:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUByte );
					break;
				}
				case 2:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUShort );
					break;
				}
				case 3:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthFloat );
					break;
				}
			}
		}
		else
		{
			OFX::BitDepthEnum bd = OFX::eBitDepthNone;
			int bitDepth         = 32; //raw_read_precision( _filePattern.getFirstFilename() );
			switch( bitDepth )
			{
				case 8:
					bd = OFX::eBitDepthUByte;
					break;
				case 16:
					bd = OFX::eBitDepthUShort;
					break;
				default:
					throw OFX::Exception::Suite( kOfxStatErrImageFormat );
					break;
			}
			clipPreferences.setClipBitDepth( *this->_dstClip, bd );
		}
		clipPreferences.setClipComponents( *this->_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setPixelAspectRatio( *this->_dstClip, 1.0 );
	}
}

}
}
}
}
