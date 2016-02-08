#include "ImageMagickReaderPluginFactory.hpp"
#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderPlugin.hpp"

#include <tuttle/ioplugin/context/ReaderPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

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

	using namespace boost::assign;
	std::vector<std::string> supportedExtensions;
	supportedExtensions += "aai", "art", "arw", "avs", "bmp", "bmp2", "bmp3",
	"cals", "cgm", "cin", "cmyk", "cmyka", "cr2", "crw", "cur", "cut", "dcm",
	"dcr", "dcx", "dib", "djvu", "dng", "dot", "dpx", "emf", "epdf", "epi", "eps",
	"eps2", "eps3", "epsf", "epsi", "ept", "exr", "fax", "fig",
	"fits", "fpx", "gif", "gplt", "gray", "hdr", "hpgl", "hrz",
	"html", "ico", "info", "inline", "jbig", "jng", "jp2", "jpc",
	"jpg", "jpeg", "man", "mat", "miff", "mono", "mng", "m2v",
	"mpc", "mpr", "mrw", "msl", "mtv", "mvg", "nef",
	"orf", "otb", "p7", "palm", "pam", "pbm", "pcd", "pcds",
	"pcl", "pcx", "pdb", "pdf", "pef", "pfa", "pfb", "pfm",
	"pgm", "picon", "pict", "pix", "png", "png8", "png16", "png32",
	"pnm", "ppm", "ps", "ps2", "ps3", "psb", "psd", "ptif",
	"pwp", "rad", "rgb", "rgba", "rla", "rle", "sct", "sfw",
	"sgi", "shtml", "sid", "mrsid", "sun", "svg", "tga", "tiff",
	"tim", "tif", "uil", "uyvy", "vicar", "viff", "wbmp",
	"wpg", "x", "xbm", "xcf", "xpm", "xwd", "x3f",
	"ycbcr", "ycbcra", "yuv";
	// blacklist
	// "avi", "mpeg", "txt", "webp", "wmf", 
	
	desc.setDescription(
			"ImageMagick File reader"
			"\n\n"
			"plugin is used to read imagemagick files."
			"\n"
			"supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );
	
	// add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	desc.setPluginEvaluation( 0 );
	
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
