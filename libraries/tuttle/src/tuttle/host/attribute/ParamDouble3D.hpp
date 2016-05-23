#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE3D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE3D_HPP_

#include "Param.hpp"
#include "ParamDoubleMultiDim.hpp"

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamDouble3D : public Param, public ParamDoubleMultiDim<3>
{
public:
    ParamDouble3D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamDouble3D* clone() const { return new ParamDouble3D(*this); }

    Ofx3DPointD getDefault() const;

    void getValue(double& x, double& y, double& z) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, double& x, double& y, double& z) const OFX_EXCEPTION_SPEC;
    void setValue(const double& x, const double& y, const double& z,
                  const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
    void setValueAtTime(const OfxTime time, const double& x, const double& y, const double& z,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValue(const int& x, const int& y, const int& z, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValue((double)x, (double)y, (double)z, change);
    }
    void setValueAtTime(const OfxTime time, const int& x, const int& y, const int& z,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, (double)x, (double)y, (double)z, change);
    }
};
}
}
}

#endif
