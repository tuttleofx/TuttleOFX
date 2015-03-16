%include <tuttle/host/global.i>

%include <tuttle/host/attribute/Param.i>
%include <tuttle/host/attribute/AnimatedParamType.i>
%include <tuttle/host/attribute/ParamDoubleMultiDim.i>

%include <tuttle/host/ofx/attribute/OfxhParam.i>
%include <tuttle/host/ofx/attribute/OfxhMultiDimParam.i>
%include <tuttle/host/ofx/attribute/OfxhParamAccessor.i>
%include <tuttle/host/ofx/attribute/OfxhParamDescriptor.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamAccessor.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamDescriptor.hpp>

#include <tuttle/host/attribute/ParamRGB.hpp>
%}

%template(ParamDoubleMultiDim3) tuttle::host::attribute::ParamDoubleMultiDim<3>;
%template(OfxhMultiDimParamParamDouble3) tuttle::host::ofx::attribute::OfxhMultiDimParam<tuttle::host::attribute::ParamDouble, 3>;

%include <tuttle/host/attribute/ParamRGB.hpp>
