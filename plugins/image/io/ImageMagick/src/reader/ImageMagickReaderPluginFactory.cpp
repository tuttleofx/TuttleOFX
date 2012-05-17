#include "ImageMagickReaderPluginFactory.hpp"
#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageMagickReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageMagickReader", "ImageMagickReader",
					"ImageMagick file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> extension;
	extension.push_back( "aai" );
	extension.push_back( "art" );
	extension.push_back( "arw" );
	extension.push_back( "avi" );
	extension.push_back( "avs" );
	extension.push_back( "bmp" );
	extension.push_back( "bmp2" );
	extension.push_back( "bmp3" );
	extension.push_back( "cals" );
	extension.push_back( "cgm" );
	extension.push_back( "cin" );
	extension.push_back( "cmyk" );
	extension.push_back( "cmyka" );
	extension.push_back( "cr2" );
	extension.push_back( "crw" );
	extension.push_back( "cur" );
	extension.push_back( "cut" );
	extension.push_back( "dcm" );
	extension.push_back( "dcr" );
	extension.push_back( "dcx" );
	extension.push_back( "dib" );
	extension.push_back( "djvu" );
	extension.push_back( "dng" );
	extension.push_back( "dot" );
	extension.push_back( "dpx" );
	extension.push_back( "emf" );
	extension.push_back( "epdf" );
	extension.push_back( "epi" );
	extension.push_back( "eps" );
	extension.push_back( "eps2" );
	extension.push_back( "eps3" );
	extension.push_back( "epsf" );
	extension.push_back( "epsi" );
	extension.push_back( "ept" );
	extension.push_back( "exr" );
	extension.push_back( "fax" );
	extension.push_back( "fig" );
	extension.push_back( "fits" );
	extension.push_back( "fpx" );
	extension.push_back( "gif" );
	extension.push_back( "gplt" );
	extension.push_back( "gray" );
	extension.push_back( "hdr" );
	extension.push_back( "hpgl" );
	extension.push_back( "hrz" );
	extension.push_back( "html" );
	extension.push_back( "ico" );
	extension.push_back( "info" );
	extension.push_back( "inline" );
	extension.push_back( "jbig" );
	extension.push_back( "jng" );
	extension.push_back( "jp2" );
	extension.push_back( "jpc" );
	extension.push_back( "jpg" );
	extension.push_back( "jpeg" );
	extension.push_back( "man" );
	extension.push_back( "mat" );
	extension.push_back( "miff" );
	extension.push_back( "mono" );
	extension.push_back( "mng" );
	extension.push_back( "m2v" );
	extension.push_back( "mpeg" );
	extension.push_back( "mpc" );
	extension.push_back( "mpr" );
	extension.push_back( "mrw" );
	extension.push_back( "msl" );
	extension.push_back( "mtv" );
	extension.push_back( "mvg" );
	extension.push_back( "nef" );
	extension.push_back( "orf" );
	extension.push_back( "otb" );
	extension.push_back( "p7" );
	extension.push_back( "palm" );
	extension.push_back( "pam" );
	extension.push_back( "pbm" );
	extension.push_back( "pcd" );
	extension.push_back( "pcds" );
	extension.push_back( "pcl" );
	extension.push_back( "pcx" );
	extension.push_back( "pdb" );
	extension.push_back( "pdf" );
	extension.push_back( "pef" );
	extension.push_back( "pfa" );
	extension.push_back( "pfb" );
	extension.push_back( "pfm" );
	extension.push_back( "pgm" );
	extension.push_back( "picon" );
	extension.push_back( "pict" );
	extension.push_back( "pix" );
	extension.push_back( "png" );
	extension.push_back( "png8" );
	extension.push_back( "png16" );
	extension.push_back( "png32" );
	extension.push_back( "pnm" );
	extension.push_back( "ppm" );
	extension.push_back( "ps" );
	extension.push_back( "ps2" );
	extension.push_back( "ps3" );
	extension.push_back( "psb" );
	extension.push_back( "psd" );
	extension.push_back( "ptif" );
	extension.push_back( "pwp" );
	extension.push_back( "rad" );
	extension.push_back( "rgb" );
	extension.push_back( "rgba" );
	extension.push_back( "rla" );
	extension.push_back( "rle" );
	extension.push_back( "sct" );
	extension.push_back( "sfw" );
	extension.push_back( "sgi" );
	extension.push_back( "shtml" );
	extension.push_back( "sid" );
	extension.push_back( "mrsid" );
	extension.push_back( "sun" );
	extension.push_back( "svg" );
	extension.push_back( "tga" );
	extension.push_back( "tiff" );
	extension.push_back( "tim" );
	extension.push_back( "tif" );
	extension.push_back( "txt" );
	extension.push_back( "uil" );
	extension.push_back( "uyvy" );
	extension.push_back( "vicar" );
	extension.push_back( "viff" );
	extension.push_back( "wbmp" );
	extension.push_back( "webp" );
	extension.push_back( "wmf" );
	extension.push_back( "wpg" );
	extension.push_back( "x" );
	extension.push_back( "xbm" );
	extension.push_back( "xcf" );
	extension.push_back( "xpm" );
	extension.push_back( "xwd" );
	extension.push_back( "x3f" );
	extension.push_back( "ycbcr" );
	extension.push_back( "ycbcra" );
	extension.push_back( "yuv" );
	
	std::string listOfExt;
	for( unsigned int i=0; i< extension.size(); i++ )
	{
		listOfExt += extension.at(i);
		listOfExt += ", ";
	}
	listOfExt.erase( listOfExt.size()-2, 2 );
	
	desc.setDescription( "ImageMagick File reader"
						 "\n\n"
						 "plugin is used to read imagemagick files."
						 "\n"
						 "supported extensions: \n" +
						 listOfExt
	);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );
	
	// add supported extensions
	for( unsigned int i=0; i< extension.size(); i++ )
	{
		desc.addSupportedExtension( extension.at(i) );
	}
	
	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ImageMagickReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
							OFX::EContext               context )
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageMagickReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
								  OFX::EContext        context )
{
    return new ImageMagickReaderPlugin( handle );
}

}
}
}
}
