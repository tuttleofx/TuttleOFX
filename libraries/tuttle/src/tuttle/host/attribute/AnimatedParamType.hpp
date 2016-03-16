#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAMTYPE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAMTYPE_HPP_

/* Template class for an animated parameter.

Used for ParamDouble and ParamInteger */

#include "AnimatedParam.hpp"

namespace tuttle
{
namespace host
{
namespace attribute
{

/**
 * @brief This class is just a re-implementation dedicated to Double Param.
 *        It allows implicit conversion from Int to Double.
 */
class AnimatedParamDouble : public AnimatedParam<double, ofx::attribute::OfxhParamDouble>
{
    typedef AnimatedParam<double, ofx::attribute::OfxhParamDouble> Parent;

public:
    AnimatedParamDouble(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                        const std::size_t index, const double value)
        : Parent(effect, name, descriptor, index, value)
    {
    }

    inline void setValue(const double& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        Parent::setValue(v, change);
    }

    inline void setValueAtTime(const OfxTime time, const double& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        Parent::setValueAtTime(time, v, change);
    }

    inline void setValue(const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        Parent::setValue((double)v, change);
    }

    inline void setValueAtTime(const OfxTime time, const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        Parent::setValueAtTime(time, (double)v, change);
    }
};

typedef AnimatedParam<int, ofx::attribute::OfxhParamInteger> AnimatedParamInteger;
}
}
}

#endif
