#include "OfxhOverlayInteract.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

OverlayInteract::OverlayInteract( imageEffect::OfxhImageEffectNode& effect, int bitDepthPerComponent, bool hasAlpha )
	: interact::OfxhInteract( effect.getOverlayDescriptor(), ( void* )( effect.getHandle() ) )
	, _instance( effect )
	, _bitDepthPerComponent( bitDepthPerComponent )
	, _hasAlpha( hasAlpha )
{
	effect.initOverlayDescriptor( bitDepthPerComponent, hasAlpha );
	createInstanceAction();
}

void OverlayInteract::draw( OfxTime time, const OfxPointD& renderScale )
{
	interact::OfxhInteract::drawAction( time, renderScale );
}

}
}
}
}

