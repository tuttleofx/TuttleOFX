%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhPluginAPICache.i>
%include <tuttle/host/ofx/OfxhPlugin.i>
%include <tuttle/host/ofx/OfxhPluginBinary.i>

%include <std_list.i>
%include <std_set.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
%}

namespace std {
%template(OfxhPluginPtrList) list<tuttle::host::ofx::OfxhPlugin*>;
}

%include <tuttle/host/ofx/OfxhPluginCache.hpp>

