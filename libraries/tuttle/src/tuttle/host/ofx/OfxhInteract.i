%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhPluginDesc.i>
%include <tuttle/host/ofx/OfxhPluginBinary.i>

%{
#include <tuttle/host/ofx/OfxhInteract.hpp>
%}

namespace tuttle {
namespace host {
namespace ofx {
namespace interact {

%feature("director") OfxhInteract;

}
}
}
}

%include <tuttle/host/ofx/OfxhInteract.hpp>




