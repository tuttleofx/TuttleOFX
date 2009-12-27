#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECT_OVERLAYINTERACT_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECT_OVERLAYINTERACT_HPP_

#include "OfxhInteract.hpp"
#include "OfxhImageEffectNode.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

/**
 * An overlay interact for image effects, derived from one of these to
 * be an overlay interact
 */
class OverlayInteract : public interact::OfxhInteract
{
protected:
	/// our image effect instance
	imageEffect::OfxhImageEffectNode& _instance;
	int _bitDepthPerComponent;
	bool _hasAlpha;

public:
	/// ctor this calls Instance->getOverlayDescriptor to get the descriptor
	OverlayInteract( imageEffect::OfxhImageEffectNode& v, int bitDepthPerComponent = 8, bool hasAlpha = false );
};

}
}
}
}

#endif

