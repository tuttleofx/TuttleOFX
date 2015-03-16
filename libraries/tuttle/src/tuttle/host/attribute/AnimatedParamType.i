%include <tuttle/host/global.i>
%include <tuttle/host/INode.i>

%include <tuttle/host/attribute/Param.i>

%include <tuttle/host/ofx/attribute/OfxhParamDouble.i>
%include <tuttle/host/ofx/attribute/OfxhParamInteger.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>

#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/AnimatedParam.hpp>
#include <tuttle/host/attribute/AnimatedParamType.hpp>
%}

%include <tuttle/host/attribute/AnimatedParam.hpp>

%template(SwigAnimatedParamDouble) tuttle::host::attribute::AnimatedParam<double, tuttle::host::ofx::attribute::OfxhParamDouble>;
%template(SwigAnimatedParamInteger) tuttle::host::attribute::AnimatedParam<int, tuttle::host::ofx::attribute::OfxhParamInteger>;

%include <tuttle/host/attribute/AnimatedParamType.hpp>
