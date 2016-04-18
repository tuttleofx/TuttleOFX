#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_

/* Most of the implementation is in the AnimatedParam template */
#include "AnimatedParamType.hpp"

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamDouble : public AnimatedParamDouble
{
public:
    ParamDouble(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                const std::size_t index = 0);

    double getDefault() const;

    /// @brief We reimplement some functions of AnimatedParamDouble, because without them,
    ///        the integer version takes the priority.
    /// @{
    inline void setValue(const double& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValue(v, change);
    }

    inline void setValueAtTime(const OfxTime time, const double& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtTime(time, v, change);
    }

    inline void setValueAtIndex(const std::size_t index, const double& v,
                                const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtIndex(index, v, change);
    }

    inline void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const double& v,
                                       const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtTimeAndIndex(time, index, v, change);
    }
    /// @}

    /// @brief Re-implementation for Int, to allows an implicit conversion from Int to Double.
    /// @{
    inline void setValue(const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValue((double)v, change);
    }

    inline void setValueAtTime(const OfxTime time, const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtTime(time, (double)v, change);
    }

    inline void setValueAtIndex(const std::size_t index, const int& v,
                                const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtIndex(index, (double)v, change);
    }

    inline void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const int& v,
                                       const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        AnimatedParamDouble::setValueAtTimeAndIndex(time, index, (double)v, change);
    }
    /// @}
};
}
}
}

#endif
