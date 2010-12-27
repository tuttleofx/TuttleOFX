#ifndef _IMAGESTATISTICSOVERLAYINTERACT_HPP_
#define _IMAGESTATISTICSOVERLAYINTERACT_HPP_

#include "ImageStatisticsPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

class ImageStatisticsOverlayInteract : public OFX::OverlayInteract
{
typedef double Scalar;

ImageStatisticsPlugin* _plugin;

interact::InteractInfos _infos;
interact::InteractScene _interactScene;

public:
	ImageStatisticsOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
};

class ImageStatisticsEffectOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new ImageStatisticsOverlayInteract( handle, effect );
	}

};

}
}
}

#endif
