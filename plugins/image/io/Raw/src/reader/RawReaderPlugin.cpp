#include "RawReaderPlugin.hpp"
#include "RawReaderProcess.hpp"
#include "RawReaderDefinitions.hpp"

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
{
	_paramFiltering = fetchChoiceParam( kParamFiltering );
}

RawReaderProcessParams RawReaderPlugin::getProcessParams( const OfxTime time )
{
	RawReaderProcessParams params;

	params._filepath  = getAbsoluteFilenameAt( time );
	params._filtering = static_cast<EFiltering>( _paramFiltering->getValue() );
	params._flip      = _paramFlip->getValue();

	return params;
}

void RawReaderPlugin::updateInfos()
{
	TUTTLE_COUT( "updateInfos begin" );
	RawReaderProcessParams params = getProcessParams( this->timeLineGetBounds().min );

	LibRaw rawProcessor;
	libraw_iparams_t& p1          = rawProcessor.imgdata.idata;
	libraw_image_sizes_t& sizes   = rawProcessor.imgdata.sizes;
	libraw_colordata_t& color     = rawProcessor.imgdata.color;
	libraw_thumbnail_t& thumbnail = rawProcessor.imgdata.thumbnail;
	libraw_imgother_t& p2         = rawProcessor.imgdata.other;
	//	libraw_output_params_t& out = rawProcessor.imgdata.params;

	if( const int ret = rawProcessor.open_file( params._filepath.c_str() ) )
	{
		TUTTLE_COUT_ERROR( "Cannot open \"" << params._filepath << "\": " << libraw_strerror( ret ) );
		return;
	}
	if( const int ret = rawProcessor.adjust_sizes_info_only() )
	{
		TUTTLE_COUT_ERROR( "Cannot decode infos \"" << params._filepath << "\": " << libraw_strerror( ret ) );
		return;
	}

	std::ostringstream ss;

	ss << "Filename: " << params._filepath << "\n";
	ss << "Timestamp: " << ctime( &( p2.timestamp ) ) << "\n";
	ss << "Camera: " << p1.make << " " << p1.model << "\n";
	if( p2.artist[0] )
		ss << "Owner: " << p2.artist << "\n";
	if( p1.dng_version )
	{
		ss << "DNG Version: ";
		for( int i = 24; i >= 0; i -= 8 )
			ss << ( p1.dng_version >> i & 255 ) << ( i ? '.' : '\n' );
		ss << "\n";
	}

	ss << "ISO speed: " << (int) p2.iso_speed << "\n";
	ss << "Shutter: ";
	if( p2.shutter > 0 && p2.shutter < 1 )
		p2.shutter = 1 / p2.shutter;
	ss << p2.shutter << " sec" << "\n"; // %0.1f
	ss << "Aperture: f/" << p2.aperture << "\n";
	ss << "Focal length: " << p2.focal_len << " mm" << "\n";
	if( color.profile )
		ss << "Embedded ICC profile: yes, " << color.profile_length << " bytes" << "\n";
	else
		ss << "Embedded ICC profile: no" << "\n";

	ss << "Number of raw images: " << p1.raw_count;
	if( sizes.pixel_aspect != 1 )
		ss << "Pixel Aspect Ratio: " << sizes.pixel_aspect << "\n";
	if( thumbnail.tlength )
		ss << "Thumb size:  " << thumbnail.twidth << " x " << thumbnail.theight << "\n";
	ss << "Full size:   " << sizes.raw_width << " x " << sizes.raw_height << "\n";

	ss << "Image size:  " << sizes.width << " x " << sizes.height << "\n";
	ss << "Output size: " << sizes.iwidth << " x " << sizes.iheight << "\n";
	ss << "Raw colors: " << p1.colors << "\n";
	if( p1.filters )
	{
		ss << "Filter pattern: ";
		if( !p1.cdesc[3] )
			p1.cdesc[3] = 'G';
		for( int i = 0; i < 16; ++i )
			putchar( p1.cdesc[rawProcessor.fc( i >> 1, i & 1 )] );
		ss << "\n";
	}
	ss << "Daylight multipliers: ";
	for( int c = 0; c < p1.colors; ++c )
		ss << " " << color.pre_mul[c];
	ss << "\n";
	if( color.cam_mul[0] > 0 )
	{
		ss << "Camera multipliers: ";
		for( int c = 0; c < 4; ++c )
			ss << " " << color.cam_mul[c];
		ss << "\n";
	}
	const char* csl[] = { "U", "I", "CO", "L", "CA" };
	ss << "Color sources /Legend: (U)nknown, (I)nit, (CO)nstant, (L)oaded, (CA)lculated/:" << "\n";
	ss << "\tcurve=" << csl[color.color_flags.curve_state] << ",";
	ss << " rgb_cam=" << csl[color.color_flags.rgb_cam_state] << ",";
	ss << " cmatrix=" << csl[color.color_flags.cmatrix_state] << ",";
	ss << " pre_mul=" << csl[color.color_flags.pre_mul_state] << ",";
	ss << " cam_mul=" << csl[color.color_flags.cam_mul_state] << "\n";
	ss << "Cam->XYZ matrix:" << "\n";
	for( int i = 0; i < 4; ++i )
		ss << color.cam_xyz[i][0] << "\t" << color.cam_xyz[i][1] << "\t" << color.cam_xyz[i][2] << "\n"; // %6.4f

	std::cout << ss;
}

void RawReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
//	else if( paramName == kRawReaderUpdateInfosButton )
//	{
//		updateInfos();
//	}
//	else
//	{
	ReaderPlugin::changedParam( args, paramName );
//	}
}

bool RawReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	updateInfos();

	RawReaderProcessParams params = getProcessParams( this->timeLineGetBounds().min );

	LibRaw rawProcessor;
	libraw_image_sizes_t& sizes = rawProcessor.imgdata.sizes;
	//libraw_output_params_t& out = rawProcessor.imgdata.params;
	//	out.half_size  = 1;

	if( const int ret = rawProcessor.open_file( params._filepath.c_str() ) )
	{
		TUTTLE_COUT_ERROR( "Cannot open \"" << params._filepath << "\": " << libraw_strerror( ret ) );
		return false;
	}
	if( const int ret = rawProcessor.adjust_sizes_info_only() )
	{
		TUTTLE_COUT_ERROR( "Cannot decode infos \"" << params._filepath << "\": " << libraw_strerror( ret ) );
		return false;
	}

	//	point2<ptrdiff_t> dims( sizes.raw_width, sizes.raw_height );
	point2<ptrdiff_t> dims( sizes.width, sizes.height );
	TUTTLE_COUT_VAR( dims );
	rod.x1 = 0;
	rod.x2 = dims.x * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dims.y;
	return true;
}

void RawReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
//	const std::string filename( getAbsoluteFirstFilename() );
	switch( getExplicitConversion() )
	{
		case eParamReaderExplicitConversionAuto:
		{
			OFX::EBitDepth bd = OFX::eBitDepthNone;
			int bitDepth      = 32;    //raw_read_precision( filename );
			switch( bitDepth )
			{
				case 8:
					bd = OFX::eBitDepthUByte;
					break;
				case 16:
					bd = OFX::eBitDepthUShort;
					break;
				case 32:
					bd = OFX::eBitDepthFloat;
					break;
				default:
					BOOST_THROW_EXCEPTION( exception::ImageFormat() );
			}
			clipPreferences.setClipBitDepth( *this->_clipDst, bd );
			break;
		}
		case eParamReaderExplicitConversionByte:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
			break;
		}
		case eParamReaderExplicitConversionShort:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
			break;
		}
		case eParamReaderExplicitConversionFloat:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			break;
		}
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void RawReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );
	doGilRender<RawReaderProcess>( *this, args );
}

}
}
}
}
