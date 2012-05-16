#ifndef _TUTTLE_PLUGIN_WRITERPLUGINFACTORY_HPP_
#define	_TUTTLE_PLUGIN_WRITERPLUGINFACTORY_HPP_

#include "WriterDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {

void describeWriterParamsInContext( OFX::ImageEffectDescriptor& desc,
				    OFX::EContext               context )
{
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
	filename->setLabel( kTuttlePluginFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* channel = desc.defineChoiceParam( kTuttlePluginChannel );
	channel->setLabel( kTuttlePluginChannelLabel );
	channel->appendOption( kTuttlePluginChannelGray );
	channel->appendOption( kTuttlePluginChannelRGB );
	channel->appendOption( kTuttlePluginChannelRGBA );
	channel->setDefault( 2 );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
	bitDepth->setLabel( kTuttlePluginBitDepthLabel );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->appendOption( kTuttlePluginBitDepth16 );
	bitDepth->setDefault( 0 );
	
	OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam( kParamPremultiplied );
	premult->setLabel( kParamPremultipliedLabel );
	premult->setDefault( false );
	
	OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam( kParamWriterRender );
	render->setLabels( "Render", "Render", "Render step" );
	render->setHint("Force render (writing)");

	OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kParamWriterRenderAlways );
	renderAlways->setLabel( "Render always" );
	renderAlways->setDefault( false );

	OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam( kParamWriterForceNewRender );
	forceNewRender->setLabel( "Force new render" );
	forceNewRender->setEnabled( false );
	forceNewRender->setIsSecret( true );
	forceNewRender->setIsPersistant( false );
	forceNewRender->setAnimates( false );
	forceNewRender->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	forceNewRender->setEvaluateOnChange( true );
	forceNewRender->setDefault( 0 );
}

}
}


#endif

