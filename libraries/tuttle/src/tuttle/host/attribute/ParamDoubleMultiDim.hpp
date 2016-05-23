#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_MULTIDIM_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_MULTIDIM_HPP_

/* Most of the implementation is in the AnimatedParam template */
#include "ParamDouble.hpp"

#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

template <std::size_t DIM>
class ParamDoubleMultiDim : public ofx::attribute::OfxhMultiDimParam<ParamDouble, DIM>
{
public:
    typedef ofx::attribute::OfxhMultiDimParam<ParamDouble, DIM> MultiDimParamParent;

    ParamDoubleMultiDim(const ofx::attribute::OfxhParamDescriptor& descriptor, const std::string& name,
                        ofx::attribute::OfxhParamSet& setInstance)
        : MultiDimParamParent(descriptor, name, setInstance)
    {
    }

    virtual ~ParamDoubleMultiDim() = 0;

    void setValueAtIndex(const std::size_t index, const double& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        MultiDimParamParent::setValueAtIndex(index, v, change);
    }
    void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const double& v,
                                const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        MultiDimParamParent::setValueAtTimeAndIndex(time, index, v, change);
    }
    void setValueAtIndex(const std::size_t index, const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        MultiDimParamParent::setValueAtIndex(index, (double)v, change);
    }
    void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const int& v,
                                const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        MultiDimParamParent::setValueAtTimeAndIndex(time, index, (double)v, change);
    }
};

template <std::size_t DIM>
ParamDoubleMultiDim<DIM>::~ParamDoubleMultiDim()
{
}
}
}
}

#endif
