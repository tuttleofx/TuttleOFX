%include <tuttle/host/global.i>
%include <tuttle/host/Preferences.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/ofx/OfxhPlugin.i>
%include <tuttle/host/ofx/OfxhPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/common/patterns/Singleton.i>

%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/host/Core.hpp>
%}

%template(SingletonCore) Singleton<tuttle::host::Core>;

%include <tuttle/host/Core.hpp>

