%include <tuttle/host/core/Exception.i>
%include <tuttle/host/ofx/property/OfxhProperty.i>

%{
#include <tuttle/host/ofx/property/OfxhPropertyTemplate.hpp>
%}

%include <tuttle/host/ofx/property/OfxhPropertyTemplate.hpp>


%template(OfxPropertyInt) tuttle::host::ofx::property::OfxhPropertyTemplate<tuttle::host::ofx::property::OfxhIntValue>;
%template(OfxPropertyDouble) tuttle::host::ofx::property::OfxhPropertyTemplate<tuttle::host::ofx::property::OfxhDoubleValue>;
%template(OfxPropertyString) tuttle::host::ofx::property::OfxhPropertyTemplate<tuttle::host::ofx::property::OfxhStringValue>;
%template(OfxPropertyPointer) tuttle::host::ofx::property::OfxhPropertyTemplate<tuttle::host::ofx::property::OfxhPointerValue>;
