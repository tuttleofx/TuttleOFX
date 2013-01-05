%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhImageEffectNode.i>
%include <tuttle/host/ofx/OfxhInteract.i>

%{
#include <tuttle/host/ofx/OfxhOverlayInteract.hpp>
%}

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

%feature("director") OverlayInteract;

}
}
}
}

%include <tuttle/host/ofx/OfxhOverlayInteract.hpp>




