%include <tuttle/host/global.i>
%include <tuttle/host/ImageEffectNode.i>
%include <tuttle/host/Graph.i>
%include <tuttle/host/ofx/OfxhInteract.i>

%{
#include <tuttle/host/OverlayInteract.hpp>
%}

namespace tuttle {
namespace host {

%feature("director") OverlayInteract;

}
}

%include <tuttle/host/OverlayInteract.hpp>

