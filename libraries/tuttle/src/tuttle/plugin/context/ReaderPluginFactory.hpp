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
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamReaderFilename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam( kParamReaderExplicitConversion );
	explicitConversion->setLabel( "Explicit conversion" );
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
		explicitConversion->setDefault( static_cast<int>( OFX::getImageEffectHostDescription()->getPixelDepth() ) );
	}

	OFX::BooleanParamDescriptor* flip = desc.defineBooleanParam( kParamReaderFlip );
	flip->setLabel( "Flip" );
	flip->setHint( "Vertically flip the image." );
	//bool hostFlip = OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke";
	//flip->setDefault( hostFlip );
	flip->setDefault( true );
}

}
}


#endif

