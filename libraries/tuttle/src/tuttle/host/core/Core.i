%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%import <tuttle/common/patterns/Singleton.i>
%template(SingletonCore) Singleton<tuttle::host::core::Core>;

%{
#include <tuttle/host/core/Core.hpp>
%}

%include <tuttle/host/core/Core.hpp>

