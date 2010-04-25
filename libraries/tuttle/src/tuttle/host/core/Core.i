%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/host/core/Core.hpp>
%}

%include <tuttle/host/core/Core.hpp>

%import <tuttle/common/patterns/Singleton.i>
%template(SingletonCore) Singleton<Core>;
