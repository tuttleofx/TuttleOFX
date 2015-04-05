%include <tuttle/host/global.i>

%include <tuttle/host/attribute/Param.i>
%include <tuttle/host/attribute/ParamInteger.i>
%include <tuttle/host/attribute/AnimatedParamType.i>

%include <tuttle/host/ofx/attribute/OfxhParam.i>
%include <tuttle/host/ofx/attribute/OfxhMultiDimParam.i>
%include <tuttle/host/ofx/attribute/OfxhParamAccessor.i>
%include <tuttle/host/ofx/attribute/OfxhParamDescriptor.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamAccessor.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamDescriptor.hpp>

#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>

#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ParamInteger.hpp>
#include <tuttle/host/attribute/ParamInteger3D.hpp>
%}

%template(OfxhMultiDimParamParamInteger3) tuttle::host::ofx::attribute::OfxhMultiDimParam<tuttle::host::attribute::ParamInteger, 3>;

%include <tuttle/host/attribute/ParamInteger3D.hpp>
