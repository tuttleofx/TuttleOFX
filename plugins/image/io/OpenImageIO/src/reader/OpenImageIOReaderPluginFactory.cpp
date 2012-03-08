#include "OpenImageIOReaderPluginFactory.hpp"
#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

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

    std::vector<std::string> extension;
    extension.push_back( "bmp" );
    extension.push_back( "cin" );
    extension.push_back( "dds" );
    extension.push_back( "dpx" );
    extension.push_back( "exr" );
    extension.push_back( "fits" );
    extension.push_back( "hdr" );
    extension.push_back( "ico" );
    extension.push_back( "j2k" );
    extension.push_back( "j2c" );
    extension.push_back( "jp2" );
    extension.push_back( "jpeg" );
    extension.push_back( "jpg" );
    extension.push_back( "jpe" );
    extension.push_back( "jfif" );
    extension.push_back( "jfi" );
    extension.push_back( "pbm" );
    extension.push_back( "pgm" );
    extension.push_back( "png" );
    extension.push_back( "pnm" );
    extension.push_back( "ppm" );
    extension.push_back( "pic" );
    extension.push_back( "psd" );
    extension.push_back( "rgbe" );
    extension.push_back( "sgi" );
    extension.push_back( "tga" );
    extension.push_back( "tif" );
    extension.push_back( "tiff" );
    extension.push_back( "tpic" );
    extension.push_back( "tx" );
    extension.push_back( "webp" );

    std::string listOfExt;
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	listOfExt += extension.at(i);
	listOfExt += ", ";
    }
    listOfExt.erase( listOfExt.size()-2, 2 );

    desc.setDescription(
	"OpenImageIO Reader"
	"\n\n"
	"Compression is only available for Exr format."
	"\n"
	"supported extensions: \n" +
	listOfExt
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
