#include "OpenImageIOReaderPluginFactory.hpp"
#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OpenImageIOReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleOpenImageIOReader", "OpenImageIOReader",
					"OpenImageIO file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );
	
	using namespace boost::assign;
	std::vector<std::string> supportedExtensions;
	supportedExtensions += "bmp", "cin", "dds", "dpx", "exr", "fits", "hdr", "ico", 
			"j2k", "j2c", "jp2", "jpeg", "jpg", "jpe", "jfif", "jfi", 
			"pbm", "pgm", "png", "pnm", "ppm", "pic", "psd", "rgbe", 
			"sgi", "tga", "tif", "tiff", "tpic", "tx", "webp", "cr2";
	
	desc.setDescription(
				"OpenImageIO Reader"
				"\n\n"
				"Compression is only available for Exr format."
				"\n"
				"supported extensions: \n" +
				boost::algorithm::join( supportedExtensions, ", " )
				);
	
	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGenerator );
	desc.addSupportedContext( OFX::eContextGeneral );
	
	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	
	// add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	desc.setPluginEvaluation( 40 );
	
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
void OpenImageIOReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
														OFX::EContext               context )
{
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
OFX::ImageEffect* OpenImageIOReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
																  OFX::EContext        context )
{
	return new OpenImageIOReaderPlugin( handle );
}

}
}
}
}
