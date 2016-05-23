#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE2D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE2D_HPP_

#include "Param.hpp"
#include "ParamDoubleMultiDim.hpp"

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamDouble2D : public Param, public ParamDoubleMultiDim<2>
{
public:
    ParamDouble2D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamDouble2D* clone() const { return new ParamDouble2D(*this); }

    OfxPointD getDefault() const;

    void getValue(double& x, double& y) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, double& x, double& y) const OFX_EXCEPTION_SPEC;

    void setValue(const double& x, const double& y, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
    void setValueAtTime(const OfxTime time, const double& x, const double& y,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValue(const int& x, const int& y, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValue((double)x, (double)y, change);
    }
    void setValueAtTime(const OfxTime time, const int& x, const int& y,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, (double)x, (double)y, change);
    }
};
}
}
}

#endif
