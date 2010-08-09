%include <tuttle/host/global.i>
%import <tuttle/host/ofx/OfxhPluginCache.i>
%import <tuttle/common/patterns/Singleton.i>
%template(SingletonCore) Singleton<tuttle::host::Core>;

%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/host/Core.hpp>
%}

%include <tuttle/host/Core.hpp>

