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

	OFX::BooleanParamDescriptor* flip = desc.defineBooleanParam( kParamWriterFlip );
	flip->setLabel( "Flip" );
	flip->setHint( "Vertically flip the image." );
//	bool hostFlip = OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke";
//	flip->setDefault( hostFlip );
	flip->setDefault( true );
}

}
}


#endif

