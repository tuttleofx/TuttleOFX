%include <tuttle/host/global.i>

%include <tuttle/host/ofx/attribute/OfxhParam.i>
%include <tuttle/host/ofx/attribute/OfxhMultiDimParam.i>
%include <tuttle/host/ofx/attribute/OfxhParamAccessor.i>
%include <tuttle/host/ofx/attribute/OfxhParamDescriptor.i>
%include <tuttle/host/ofx/attribute/OfxhKeyframeParam.i>

%include <tuttle/host/attribute/ParamDouble.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamAccessor.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamDescriptor.hpp>

#include <tuttle/host/attribute/ParamDoubleMultiDim.hpp>
%}

%include <tuttle/host/attribute/ParamDoubleMultiDim.hpp>

namespace tuttle {
namespace host {


namespace ofx {
namespace attribute {
%template(OfxhMultiDimParamParamDouble2) OfxhMultiDimParam<tuttle::host::attribute::ParamDouble, 2>;
%template(OfxhMultiDimParamParamDouble3) OfxhMultiDimParam<tuttle::host::attribute::ParamDouble, 3>;
%template(OfxhMultiDimParamParamDouble4) OfxhMultiDimParam<tuttle::host::attribute::ParamDouble, 4>;
}
}

namespace attribute {
%template(ParamDoubleMultiDim2) ParamDoubleMultiDim<2>;
%template(ParamDoubleMultiDim3) ParamDoubleMultiDim<3>;
%template(ParamDoubleMultiDim4) ParamDoubleMultiDim<4>;
}

}
}

