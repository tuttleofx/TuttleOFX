#include "TextPlugin.hpp"
#include "TextDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <limits>

namespace tuttle {
namespace plugin {
namespace text {

static const bool kSupportTiles = false;

mDeclarePluginFactory( TextPluginFactory, { }, { } );


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TextPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleText", "Text",
				   "Text" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setSupportsMultiResolution( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TextPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::ContextEnum context )
{
	OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::StringParamDescriptor *text = desc.defineStringParam( kText );
	text->setLabel( "Text" );
	text->setStringType( OFX::eStringTypeMultiLine );

	OFX::StringParamDescriptor *font = desc.defineStringParam( kFont );
	font->setLabel( "Font file" );
	font->setStringType( OFX::eStringTypeFilePath );
	font->setDefault( "/usr/share/fonts/truetype/msttcorefonts/arial.ttf" );

	OFX::IntParamDescriptor* size = desc.defineIntParam( kFontSize );
	size->setLabel( "Size" );
	size->setDefault( 18 );
	size->setRange( 0, std::numeric_limits<int>::max() );
	size->setDisplayRange( 0, 60 );

	OFX::RGBAParamDescriptor* color = desc.defineRGBAParam( kFontColor );
	color->setLabel( "Color" );
	color->setDefault( 1.0, 1.0, 1.0, 1.0 );

}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TextPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new TextPlugin( handle );
}

}
}
}

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::text::TextPluginFactory p( "fr.tuttle.text", 1, 0 );
	ids.push_back( &p );
}

}
}
