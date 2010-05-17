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


void RawReaderPlugin::updateInfos()
{
	RawReaderProcessParams params = getProcessParams( this->timeLineGetBounds().min );

	LibRaw rawProcessor;
	libraw_iparams_t& p1 = rawProcessor.imgdata.idata;
	libraw_image_sizes_t& sizes = rawProcessor.imgdata.sizes;
	libraw_colordata_t& color = rawProcessor.imgdata.color;
	libraw_thumbnail_t& thumbnail = rawProcessor.imgdata.thumbnail;
	libraw_imgother_t& p2 = rawProcessor.imgdata.other;
	libraw_output_params_t& out = rawProcessor.imgdata.params;

	if( const int ret = rawProcessor.open_file( params._filepath.c_str() ) )
	{
		COUT_ERROR( "Cannot open \"" << params._filepath << "\": " << libraw_strerror(ret) );
		return;
	}
	if( const int ret = rawProcessor.adjust_sizes_info_only() )
	{
		COUT_ERROR( "Cannot decode infos \"" << params._filepath << "\": " << libraw_strerror( ret ) );
		return;
	}

	std::ostringstream ss;
	
	ss << "Filename: " << params._filepath << std::endl;
	ss << "Timestamp: " << ctime( &( p2.timestamp ) ) << std::endl;
	ss << "Camera: " << p1.make << " " << p1.model << std::endl;
	if( p2.artist[0] )
		ss << "Owner: " << p2.artist << std::endl;
	if( p1.dng_version )
	{
		ss << "DNG Version: ";
		for( int i = 24; i >= 0; i -= 8 )
			ss << (p1.dng_version >> i & 255) << (i ? '.' : '\n');
		ss << std::endl;
	}
	
	ss << "ISO speed: " << (int) p2.iso_speed << std::endl;
	ss << "Shutter: ";
	if( p2.shutter > 0 && p2.shutter < 1 )
		p2.shutter = 1 / p2.shutter;
	ss << p2.shutter << " sec" << std::endl; // %0.1f
	ss << "Aperture: f/" << p2.aperture << std::endl;
	ss << "Focal length: " << p2.focal_len << " mm" << std::endl;
	if( color.profile )
		ss << "Embedded ICC profile: yes, " << color.profile_length << " bytes" << std::endl;
	else
		ss << "Embedded ICC profile: no" << std::endl;

	ss << "Number of raw images: " << p1.raw_count;
	if( sizes.pixel_aspect != 1 )
		ss << "Pixel Aspect Ratio: " << sizes.pixel_aspect << std::endl;
	if( thumbnail.tlength )
		ss << "Thumb size:  " << thumbnail.twidth << " x " << thumbnail.theight << std::endl;
	ss << "Full size:   " << sizes.raw_width << " x " << sizes.raw_height << std::endl;

	ss << "Image size:  " << sizes.width << " x " << sizes.height << std::endl;
	ss << "Output size: " << sizes.iwidth << " x " << sizes.iheight << std::endl;
	ss << "Raw colors: " << p1.colors << std::endl;
	if( p1.filters )
	{
		ss << "Filter pattern: ";
		if( !p1.cdesc[3] )
			p1.cdesc[3] = 'G';
		for( int i = 0; i < 16; ++i )
			putchar( p1.cdesc[rawProcessor.fc( i >> 1, i & 1 )] );
		ss << std::endl;
	}
	ss << "Daylight multipliers: ";
	for( int c = 0; c < p1.colors; ++c )
		ss << " " << color.pre_mul[c];
	ss << std::endl;
	if( color.cam_mul[0] > 0 )
	{
		ss << "Camera multipliers: ";
		for( int c = 0; c < 4; ++c )
			ss << " " << color.cam_mul[c];
		ss << std::endl;
	}
	const char* csl[] = { "U", "I", "CO", "L", "CA" };
	ss << "Color sources /Legend: (U)nknown, (I)nit, (CO)nstant, (L)oaded, (CA)lculated/:" << std::endl;
	ss << "\tcurve=" << csl[color.color_flags.curve_state] << ",";
	ss << " rgb_cam=" << csl[color.color_flags.rgb_cam_state] << ",";
	ss << " cmatrix=" << csl[color.color_flags.cmatrix_state] << ",";
	ss << " pre_mul=" << csl[color.color_flags.pre_mul_state] << ",";
	ss << " cam_mul="<< csl[color.color_flags.cam_mul_state] << std::endl;
	ss << "Cam->XYZ matrix:" << std::endl;
	for( int i = 0; i < 4; i++ )
		ss << color.cam_xyz[i][0] << "\t" << color.cam_xyz[i][1] << "\t" << color.cam_xyz[i][2] << std::endl; // %6.4f

	std::cout << ss;
}

void RawReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kRawReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kRawReaderHelpString );
	}
//	else if( paramName == kRawReaderUpdateInfosButton )
//	{
//		updateInfos();
//	}
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
