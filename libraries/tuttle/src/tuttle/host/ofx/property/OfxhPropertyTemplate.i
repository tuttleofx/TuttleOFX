%include <tuttle/host/global.i>
%include <tuttle/host/ofx/property/OfxhProperty.i>

%{
#include <tuttle/host/ofx/property/OfxhPropertyTemplate.hpp>
%}

/*
namespace tuttle {
namespace host {
namespace ofx {
namespace property {
%template(OfxhPropertyInt) OfxhPropertyTemplate<OfxhIntValue>;
%template(OfxhPropertyDouble) OfxhPropertyTemplate<OfxhDoubleValue>;
%template(OfxhPropertyString) OfxhPropertyTemplate<OfxhStringValue>;
%template(OfxhPropertyPointer) OfxhPropertyTemplate<OfxhPointerValue>;
}
}
}
}
*/

%include <tuttle/host/ofx/property/OfxhPropertyTemplate.hpp>
