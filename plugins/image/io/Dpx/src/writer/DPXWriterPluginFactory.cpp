#include "DPXWriterPluginFactory.hpp"
#include "DPXWriterPlugin.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void DPXWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleDpxWriter", "DpxWriter",
			    "Dpx file writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	desc.setDescription( "Digital Picture Exchange (DPX), ANSI/SMPTE standard (268M-2003)" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// add supported extensions
	desc.addSupportedExtension( "dpx" );

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
void DPXWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
	filename->setLabel( kTuttlePluginFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
	bitDepth->setLabel( kTuttlePluginBitDepthLabel );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->appendOption( kTuttlePluginBitDepth10 );
	bitDepth->appendOption( kTuttlePluginBitDepth12 );
	bitDepth->appendOption( kTuttlePluginBitDepth16 );
	bitDepth->appendOption( kTuttlePluginBitDepth32 );
	bitDepth->appendOption( kTuttlePluginBitDepth64 );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitDepth->setDefault( eTuttlePluginBitDepth10 );

	OFX::ChoiceParamDescriptor* descriptor = desc.defineChoiceParam( kParamDescriptor );
	descriptor->setLabel( kParamDescriptorLabel );
	descriptor->setHint( kParamDescriptorHint );
	descriptor->appendOption( kParamDescriptorUserDefinedDescriptor );
	descriptor->appendOption( kParamDescriptorRed );
	descriptor->appendOption( kParamDescriptorGreen );
	descriptor->appendOption( kParamDescriptorBlue );
	descriptor->appendOption( kParamDescriptorAlpha );
	descriptor->appendOption( kParamDescriptorLuma );
	descriptor->appendOption( kParamDescriptorColorDifference );
	descriptor->appendOption( kParamDescriptorDepth );
	descriptor->appendOption( kParamDescriptorCompositeVideo );
	descriptor->appendOption( kParamDescriptorRGB );
	descriptor->appendOption( kParamDescriptorRGBA );
	descriptor->appendOption( kParamDescriptorABGR );
	descriptor->appendOption( kParamDescriptorCbYCrY );
	descriptor->appendOption( kParamDescriptorCbYACrYA );
	descriptor->appendOption( kParamDescriptorCbYCr );
	descriptor->appendOption( kParamDescriptorCbYCrA );
	descriptor->appendOption( kParamDescriptorUserDefined2Comp );
	descriptor->appendOption( kParamDescriptorUserDefined3Comp );
	descriptor->appendOption( kParamDescriptorUserDefined4Comp );
	descriptor->appendOption( kParamDescriptorUserDefined5Comp );
	descriptor->appendOption( kParamDescriptorUserDefined6Comp );
	descriptor->appendOption( kParamDescriptorUserDefined7Comp );
	descriptor->appendOption( kParamDescriptorUserDefined8Comp );
	descriptor->appendOption( kParamDescriptorUndefinedDescriptor );
	descriptor->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	descriptor->setDefault( 9 ); // RGB

	OFX::ChoiceParamDescriptor* transfer = desc.defineChoiceParam( kParamTransfer );
	transfer->setLabel( kParamTransferLabel );
	transfer->setHint( kParamTransferHint );
	transfer->appendOption( kParamCharacteristicUserDefined );
	transfer->appendOption( kParamCharacteristicPrintingDensity );
	transfer->appendOption( kParamCharacteristicLinear );
	transfer->appendOption( kParamCharacteristicLogarithmic );
	transfer->appendOption( kParamCharacteristicUnspecifiedVideo );
	transfer->appendOption( kParamCharacteristicSMPTE274M );
	transfer->appendOption( kParamCharacteristicITUR709 );
	transfer->appendOption( kParamCharacteristicITUR601 );
	transfer->appendOption( kParamCharacteristicITUR602 );
	transfer->appendOption( kParamCharacteristicNTSCCompositeVideo );
	transfer->appendOption( kParamCharacteristicPALCompositeVideo );
	transfer->appendOption( kParamCharacteristicZLinear );
	transfer->appendOption( kParamCharacteristicZHomogeneous );
	transfer->appendOption( kParamCharacteristicUndefinedCharacteristic );
	transfer->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	transfer->setDefault( 2 ); // Linear

	OFX::ChoiceParamDescriptor* colorimetric = desc.defineChoiceParam( kParamColorimetric );
	colorimetric->setLabel( kParamColorimetricLabel );
	colorimetric->setHint( kParamColorimetricHint );
	colorimetric->appendOption( kParamCharacteristicUserDefined );
	colorimetric->appendOption( kParamCharacteristicPrintingDensity );
	colorimetric->appendOption( kParamCharacteristicLinear );
	colorimetric->appendOption( kParamCharacteristicLogarithmic );
	colorimetric->appendOption( kParamCharacteristicUnspecifiedVideo );
	colorimetric->appendOption( kParamCharacteristicSMPTE274M );
	colorimetric->appendOption( kParamCharacteristicITUR709 );
	colorimetric->appendOption( kParamCharacteristicITUR601 );
	colorimetric->appendOption( kParamCharacteristicITUR602 );
	colorimetric->appendOption( kParamCharacteristicNTSCCompositeVideo );
	colorimetric->appendOption( kParamCharacteristicPALCompositeVideo );
	colorimetric->appendOption( kParamCharacteristicZLinear );
	colorimetric->appendOption( kParamCharacteristicZHomogeneous );
	colorimetric->appendOption( kParamCharacteristicUndefinedCharacteristic );
	colorimetric->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	colorimetric->setDefault( 2 ); // Linear

	OFX::ChoiceParamDescriptor* packed = desc.defineChoiceParam( kParamPacked );
	packed->setLabel( kParamPackedLabel );
	packed->setHint( kParamPackedHint );
	packed->appendOption( kParamPackedPacked );
	packed->appendOption( kParamPackedMethodA );
	packed->appendOption( kParamPackedMethodB );
	packed->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	packed->setDefault( 1 );

	OFX::ChoiceParamDescriptor* encoding = desc.defineChoiceParam( kParamEncoding );
	encoding->setLabel( kParamEncodingLabel );
	encoding->setHint( kParamEncodingHint );
	encoding->appendOption( kParamEncodingNone );
	encoding->appendOption( kParamEncodingRle );
	encoding->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	encoding->setDefault( 0 );

/*



		 * \param element element number (0-7)
ok	 * \param desc image descriptor
ok	 * \param bitDepth bit depth of image, valid values are [8,10,12,16,32,64]
ok	 * \param transfer transfer characteristic
ok	 * \param colorimetric colorimetric specification
ok	 * \param packing packing type
ok	 * \param encoding encoding type
		 * \param dataSign
		 * \param lowData
		 * \param lowQuantity
		 * \param highData
		 * \param highQuantity
		 * \param eolnPadding end of line padding (in bytes)
		 * \param eoimPadding end of image padding (in bytes)

		void SetElement(const int element = 0,
			const Descriptor desc = kRGB,
			const U8 bitDepth = 10,
			const Characteristic transfer = kLogarithmic,
			const Characteristic colorimetric = kLogarithmic,
			const Packing packing = kFilledMethodA,
			const Encoding encoding = kNone,
			const U32 dataSign = 0,
			const U32 lowData = ~0, const R32 lowQuantity = std::numeric_limits<float>::quiet_NaN(),
			const U32 highData = ~0, const R32 highQuantity = std::numeric_limits<float>::quiet_NaN(),
			const U32 eolnPadding = 0, const U32 eoimPadding = 0);

*/

	describeWriterParamsInContext( desc, context );

}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DPXWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							  OFX::EContext        context )
{
	return new DPXWriterPlugin( handle );
}

}
}
}
}
