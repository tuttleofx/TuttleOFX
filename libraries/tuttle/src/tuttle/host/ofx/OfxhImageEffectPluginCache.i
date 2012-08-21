%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
%include <tuttle/host/ofx/OfxhImageEffectHost.i>

%include <std_vector.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>
%}


namespace std {
typedef OfxhImageEffectPluginPtrVector vector<tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
typedef OfxhImageEffectPluginByStrMap map<std::string, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
typedef OfxhImageEffectPluginByMajorPluginMap map<OfxhMajorPlugin, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
}

%include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>



