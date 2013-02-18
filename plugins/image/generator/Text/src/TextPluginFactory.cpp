#include "TextPluginFactory.hpp"
#include "TextPlugin.hpp"
#include "TextDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#ifndef __WINDOWS__
#include <fontconfig/fontconfig.h>
#endif

#include <iostream>

#include <boost/algorithm/string.hpp>

#include <limits>

namespace tuttle {
namespace plugin {
namespace text {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TextPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleText",
					"Text",
					"Text" );
	desc.setPluginGrouping( "tuttle/image/generator" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TextPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                           OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );
	srcClip->setOptional(true);

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::StringParamDescriptor* text = desc.defineStringParam( kParamText );
	text->setLabel( "Text" );
	text->setStringType( OFX::eStringTypeMultiLine );

	OFX::BooleanParamDescriptor* isExpression = desc.defineBooleanParam( kParamIsExpression );
	isExpression->setLabel( "Expression" );
	isExpression->setHint( "If you check this parameter the text must be a python code.\n"
	                       "The final result must be in a variable with the name of the parameter.\n"
	                       "Example:\n"
	                       "from math import *\n"
	                       //+ kParamText +
	                       "text = 'At frame '+str(time)+', value is ' + str( sin(time) )\n" );
	isExpression->setDefault( false );
	
#ifdef __WINDOWS__
	OFX::StringParamDescriptor* font = desc.defineStringParam( kParamFont );
	font->setStringType( OFX::eStringTypeFilePath );
	font->setHint( "When a font file path is activate, the bold and italic options are not available." );
#else
	OFX::ChoiceParamDescriptor* font = desc.defineChoiceParam( kParamFont );

	FcInit ();
	FcConfig *config = FcInitLoadConfigAndFonts();
	FcChar8 *s;
	FcPattern *p  = FcPatternBuild( NULL, 
									FC_WEIGHT, FcTypeInteger, FC_WEIGHT_BOLD, 
									FC_SLANT, FcTypeInteger, FC_SLANT_ITALIC, 
									NULL );

	FcObjectSet *os = FcObjectSetBuild( FC_FAMILY, NULL );
	FcFontSet   *fs = FcFontList( config, p, os );

	for( int i=0; fs && i < fs->nfont; i++ )
	{
		FcPattern *fcFont = fs->fonts[ i ];
		s = FcNameUnparse( fcFont );
	
		std::ostringstream stream ;
		stream << s ;
		std::string id = stream.str();
		boost::algorithm::to_lower( id );
		boost::replace_all ( id, " ", "_" );
	
		font->appendOption( id, stream.str() );
		
		if(!strcmp((char*)s, "Arial"))
			font->setDefault( i );	
	}
#endif
	font->setHint("Select the font.");
	font->setLabel("Font");
	
	OFX::IntParamDescriptor* size = desc.defineIntParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 18 );
	size->setRange( 0, std::numeric_limits<int>::max() );
	size->setDisplayRange( 0, 60 );

	OFX::DoubleParamDescriptor* ratio = desc.defineDoubleParam( kParamRatio );
	ratio->setLabel( "Ratio" );
	ratio->setRange( 0.0, std::numeric_limits<double>::max() );
	ratio->setDisplayRange( 0.0, 2.0 );
	ratio->setDefault( 1.0 );

	OFX::RGBAParamDescriptor* color = desc.defineRGBAParam( kParamColor );
	color->setLabel( "Color" );
	color->setDefault( 1.0, 1.0, 1.0, 1.0 );

	OFX::RGBAParamDescriptor* backgroundColor = desc.defineRGBAParam( kParamBackgroundColor );
	backgroundColor->setLabel( "Background Color" );
	backgroundColor->setDefault( 0.0, 0.0, 0.0, 0.0 );

	OFX::Double2DParamDescriptor* position = desc.defineDouble2DParam( kParamPosition );
	position->setLabel( "Position" );
	position->setDefault( 0.0, 0.0 );

	OFX::DoubleParamDescriptor* letterSpacing = desc.defineDoubleParam( kParamLetterSpacing );
	letterSpacing->setLabel( "Letter spacing" );
	letterSpacing->setDisplayRange( -10.0, 10.0 );
	letterSpacing->setDefault( 0.0 );

	OFX::ChoiceParamDescriptor* vAlign = desc.defineChoiceParam( kParamVAlign );
	vAlign->setLabel( "Vertically align" );
	vAlign->appendOption( kParamVAlignTop );
	vAlign->appendOption( kParamVAlignCenter );
	vAlign->appendOption( kParamVAlignBottom );
	vAlign->setDefault( eParamVAlignCenter );

	OFX::ChoiceParamDescriptor* hAlign = desc.defineChoiceParam( kParamHAlign );
	hAlign->setLabel( "Horizontally align" );
	hAlign->appendOption( kParamHAlignLeft );
	hAlign->appendOption( kParamHAlignCenter );
	hAlign->appendOption( kParamHAlignRight );
	hAlign->setDefault( eParamHAlignCenter );

	OFX::BooleanParamDescriptor* verticalFlip = desc.defineBooleanParam( kParamVerticalFlip );
	verticalFlip->setLabel( "Vertical flip" );
	verticalFlip->setDefault( false );
	verticalFlip->setAnimates( false );
	verticalFlip->setHint( "Some hosts use inverted images, so you can correct this problem using this flag." );

	OFX::BooleanParamDescriptor* italic = desc.defineBooleanParam( kParamItalic );
	italic->setLabel( "Italic" );
	italic->setDefault( false );

	OFX::BooleanParamDescriptor* bold = desc.defineBooleanParam( kParamBold );
	bold->setLabel( "Bold" );
	bold->setDefault( false );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TextPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::EContext        context )
{
	return new TextPlugin( handle );
}

}
}
}
