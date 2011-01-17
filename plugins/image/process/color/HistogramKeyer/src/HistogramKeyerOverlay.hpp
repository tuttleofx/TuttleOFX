#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYEROVERLAY_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYEROVERLAY_HPP_

#include "HistogramKeyerPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

class HistogramKeyerOverlay : public OFX::OverlayInteract
{
typedef double Scalar;

HistogramKeyerPlugin* _plugin;

interact::InteractInfos _infos;

public:
	HistogramKeyerOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
};

class HistogramKeyerParamOverlayDescriptor : public OFX::ParamInteractDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new HistogramKeyerOverlay( handle, effect );
	}
};

class HistogramKeyerOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new HistogramKeyerOverlay( handle, effect );
	}
};

}
}
}

#endif
