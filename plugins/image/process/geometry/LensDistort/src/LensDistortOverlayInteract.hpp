#ifndef _LENSDISTORTOVERLAYINTERACT_HPP_
#define _LENSDISTORTOVERLAYINTERACT_HPP_

#include "LensDistortPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace lens {

class LensDistortOverlayInteract : public OFX::OverlayInteract
{
typedef double Scalar;

LensDistortPlugin* _plugin;

interact::InteractInfos _infos;
interact::InteractScene _interactScene;

public:
	LensDistortOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
};

class LensDistortOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new LensDistortOverlayInteract( handle, effect );
	}

};

}
}
}

#endif
