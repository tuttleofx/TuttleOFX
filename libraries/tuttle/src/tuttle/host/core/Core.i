%include <tuttle/host/core/Exception.i>
%import <tuttle/host/ofx/OfxhPluginCache.i>
%import <tuttle/common/patterns/Singleton.i>
%template(SingletonCore) Singleton<tuttle::host::core::Core>;

%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/host/core/Core.hpp>
%}

%include <tuttle/host/core/Core.hpp>

