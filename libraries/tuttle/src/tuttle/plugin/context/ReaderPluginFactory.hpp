#ifndef _TUTTLE_PLUGIN_READERPLUGINFACTORY_HPP_
#define	_TUTTLE_PLUGIN_READERPLUGINFACTORY_HPP_

#include "ReaderDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {

void describeReaderParamsInContext( OFX::ImageEffectDescriptor& desc,
				    OFX::EContext               context )
{
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
	filename->setLabel( kTuttlePluginFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* component = desc.defineChoiceParam( kTuttlePluginChannel );
	component->appendOption( kTuttlePluginChannelAuto );
	component->appendOption( kTuttlePluginChannelGray );
	component->appendOption( kTuttlePluginChannelRGB );
	component->appendOption( kTuttlePluginChannelRGBA );

	component->setLabel( kTuttlePluginChannelLabel );
	component->setDefault( eParamReaderChannelAuto );
	desc.addClipPreferencesSlaveParam( *component );

	
	OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam( kTuttlePluginBitDepth );
	explicitConversion->setLabel( kTuttlePluginBitDepthLabel );
	explicitConversion->appendOption( kTuttlePluginBitDepthAuto );
	explicitConversion->appendOption( kTuttlePluginBitDepth8 );
	explicitConversion->appendOption( kTuttlePluginBitDepth16 );
	explicitConversion->appendOption( kTuttlePluginBitDepth32f );
	explicitConversion->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	explicitConversion->setAnimates( false );
	desc.addClipPreferencesSlaveParam( *explicitConversion );

	if( OFX::getImageEffectHostDescription()->supportsMultipleClipDepths )
	{
		explicitConversion->setDefault( 0 );
	}
	else
	{
		explicitConversion->setIsSecret( true );
		explicitConversion->setDefault( static_cast<int>( OFX::getImageEffectHostDescription()->getDefaultPixelDepth() ) );
	}
}

}
}


#endif

