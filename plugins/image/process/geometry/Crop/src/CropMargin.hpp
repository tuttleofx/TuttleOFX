#ifndef _TUTTLE_PLUGIN_CROP_MARGIN_HPP_
#define _TUTTLE_PLUGIN_CROP_MARGIN_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

class CropOverlay : public OFX::OverlayInteract
{
public:
	CropOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect ) : OFX::OverlayInteract( handle )
	{
		_effect = effect;
	}

	virtual bool draw( const OFX::DrawArgs& args );
};

class CropEffectOverlay : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new CropOverlay( handle, effect );
	}

};

}
}
}

#endif
