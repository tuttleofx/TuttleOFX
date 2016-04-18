#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_

/* Most of the implementation is in the AnimatedParam template */
#include "AnimatedParamType.hpp"

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamInteger : public AnimatedParamInteger
{
public:
    ParamInteger(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                 const std::size_t index = 0);

    int getDefault() const;
};
}
}
}

#endif
