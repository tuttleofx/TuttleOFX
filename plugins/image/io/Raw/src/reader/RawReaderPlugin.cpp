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
	_paramFiltering     = fetchChoiceParam( kParamFiltering );
	_paramInterpolation = fetchChoiceParam( kParamInterpolation );
	_paramOutputColor   = fetchChoiceParam( kParamOutputColor );
	
	_paramGreyboxPoint = fetchDouble2DParam( kParamGreyboxPoint );
	_paramGreyboxSize  = fetchDouble2DParam( kParamGreyboxSize );
	
	_paramGammaPower = fetchDoubleParam( kParamGammaPower );
	_paramGammaToe   = fetchDoubleParam( kParamGammaToe );
	_paramRedAbber   = fetchDoubleParam( kParamRedAbber );
	_paramGreenAbber = fetchDoubleParam( kParamGreenAbber );
	
	_paramBright     = fetchDoubleParam( kParamBright );
	_paramAutoBright = fetchBooleanParam( kParamAutoBright );

	_paramThreshold  = fetchDoubleParam( kParamThreshold );
	_paramFourColorRgb = fetchBooleanParam( kParamFourColorRgb );
	
	_paramExposure         = fetchDoubleParam( kParamExposure );
	_paramExposurePreserve = fetchDoubleParam( kParamExposurePreserve );
	
	_paramWhiteBalance     = fetchChoiceParam( kParamWhiteBalance );
	
	_paramHighlight = fetchChoiceParam( kParamHighlight ) ;

	_paramFbddNoiseRd = fetchChoiceParam( kParamFBDDNoiseRd ) ;
	
	// metadatas
	_paramManufacturer   = fetchStringParam( kParamManufacturer );
	_paramModel          = fetchStringParam( kParamModel );
	_paramIso            = fetchIntParam   ( kParamIso );
	_paramShutter        = fetchIntParam   ( kParamShutter );
	_paramAperture       = fetchDoubleParam( kParamAperture );
	_paramDateOfShooting = fetchStringParam( kParamDateOfShooting );
	_paramGPS            = fetchStringParam( kParamGPS );
	_paramDesc           = fetchStringParam( kParamDesc );
	_paramArtist         = fetchStringParam( kParamArtist );
}

RawReaderProcessParams<RawReaderPlugin::Scalar> RawReaderPlugin::getProcessParams( const OfxTime time )
{
	RawReaderProcessParams<Scalar> params;

	params._filepath      = getAbsoluteFilenameAt( time );
	params._filtering     = static_cast<EFiltering>( _paramFiltering->getValue() );
	params._interpolation = static_cast<EInterpolation>( _paramInterpolation->getValue() );
	params._outputColor   = static_cast<EOutputColor>( _paramOutputColor->getValue() );
	
	params._gammaPower = _paramGammaPower->getValue();
	params._gammaToe   = _paramGammaToe->getValue();
	params._redAbber   = _paramRedAbber->getValue();
	params._greenAbber = _paramGreenAbber->getValue();
	
	params._bright     = _paramBright->getValue();
	params._autoBright = _paramAutoBright->getValue();

	params._threshold  = _paramThreshold->getValue();
	params._fourColorRgb = _paramFourColorRgb->getValue();
	
	params._greyboxPoint.x = _paramGreyboxPoint->getValue().x;
	params._greyboxPoint.y = _paramGreyboxPoint->getValue().y;
	
	params._greyboxSize.x = _paramGreyboxSize->getValue().x;
	params._greyboxSize.y = _paramGreyboxSize->getValue().y;
	
	params._exposure         = _paramExposure->getValue();
	params._exposurePreserve = _paramExposurePreserve->getValue();
	
	params._whiteBalance     = static_cast<EWhiteBalance>( _paramWhiteBalance->getValue() );
	
	params._hightlight = static_cast<EHighlight>( _paramHighlight->getValue() );

	params._fbddNoiseRd = static_cast<EFBDDNoiseRd>( _paramFbddNoiseRd->getValue() );
	
	return params;
}

void RawReaderPlugin::updateInfos( const OfxTime time )
{
	RawReaderProcessParams<Scalar> params = getProcessParams( time );

	LibRaw rawProcessor;
	libraw_iparams_t& p1          = rawProcessor.imgdata.idata;
	libraw_image_sizes_t& sizes   = rawProcessor.imgdata.sizes;
	libraw_colordata_t& color     = rawProcessor.imgdata.color;
	libraw_thumbnail_t& thumbnail = rawProcessor.imgdata.thumbnail;
	libraw_imgother_t& p2         = rawProcessor.imgdata.other;
	//	libraw_output_params_t& out = rawProcessor.imgdata.params;

	if( const int ret = rawProcessor.open_file( params._filepath.c_str() ) )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
			<< exception::user( "RAW: Unable to open file" )
			<< exception::dev( libraw_strerror( ret ) )
			<< exception::filename( params._filepath ) );
	}
	if( const int ret = rawProcessor.adjust_sizes_info_only() )
	{
		BOOST_THROW_EXCEPTION( exception::FileInSequenceNotExist()
			<< exception::user( "RAW: Cannot decode infos for file" )
			<< exception::dev( libraw_strerror( ret ) )
			<< exception::filename( params._filepath ) );
	}

	_paramManufacturer->setValue( p1.make );
	_paramModel->setValue( p1.model );
	_paramIso->setValue( (int) p2.iso_speed );
	
	if( p2.shutter > 0 && p2.shutter < 1 )
		p2.shutter = 1 / p2.shutter;
	_paramShutter->setValue( p2.shutter );
	_paramAperture->setValue( p2.aperture );
	_paramDateOfShooting->setValue( ctime( &( p2.timestamp ) ) );
	
	if( p2.gpsdata[0] )
		_paramGPS->setValue( (const char*)p2.gpsdata );
	if( p2.desc[0] )
		_paramDesc->setValue( p2.desc );
	if( p2.artist[0] )
		_paramArtist->setValue( p2.artist );
	
	// https://github.com/LibRaw/LibRaw/blob/master/samples/raw-identify.cpp
	
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
	/*if( p2.shutter > 0 && p2.shutter < 1 )
		p2.shutter = 1 / p2.shutter;*/
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
		{
#if LIBRAW_COMPILE_CHECK_VERSION_NOTLESS(0,15)
			putchar( p1.cdesc[rawProcessor.fcol( i >> 1, i & 1 )] );
#else
			putchar( p1.cdesc[rawProcessor.fc( i >> 1, i & 1 )] );
#endif
		}
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
	ss << "Cam->XYZ matrix:" << "\n";
	for( int i = 0; i < 4; ++i )
		ss << color.cam_xyz[i][0] << "\t" << color.cam_xyz[i][1] << "\t" << color.cam_xyz[i][2] << "\n"; // %6.4f

	TUTTLE_LOG_DEBUG( ss );
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
	updateInfos( args.time );

	RawReaderProcessParams<Scalar> params = getProcessParams( args.time );

	LibRaw rawProcessor;
	libraw_image_sizes_t& sizes = rawProcessor.imgdata.sizes;
	//libraw_output_params_t& out = rawProcessor.imgdata.params;
	//	out.half_size  = 1;

	if( rawProcessor.open_file( params._filepath.c_str() ) )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
			<< exception::user( "RAW: Unable to open file" )
			<< exception::filename( params._filepath ) );
	}
	if( rawProcessor.adjust_sizes_info_only() )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "RAW: Cannot decode infos" )
			<< exception::filename( params._filepath ) );
	}

	//	point2<ptrdiff_t> dims( sizes.raw_width, sizes.raw_height );
	point2<ptrdiff_t> dims( sizes.width, sizes.height );
	//TUTTLE_LOG_VAR( TUTTLE_INFO, dims );
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
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
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
