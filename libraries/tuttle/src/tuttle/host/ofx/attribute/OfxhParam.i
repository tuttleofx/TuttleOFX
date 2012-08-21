%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhAttribute.i>

%apply int { OfxTime };
%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
%}

%include <tuttle/host/ofx/attribute/OfxhParam.hpp>


