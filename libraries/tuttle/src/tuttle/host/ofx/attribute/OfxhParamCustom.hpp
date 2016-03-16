#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMCUSTOM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMCUSTOM_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"

#include "OfxhParamString.hpp" /// @todo tuttle: custom parameter support...

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhParamCustom : public OfxhParamString
{
public:
    OfxhParamCustom(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance)
        : OfxhParamString(descriptor, name, setInstance)
    {
    }
};
}
}
}
}

#endif
