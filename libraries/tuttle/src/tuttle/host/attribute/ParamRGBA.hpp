#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_

#include "Param.hpp"

#include "ParamDoubleMultiDim.hpp"

#include <ofxPixels.h>

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamRGBA : public Param, public ParamDoubleMultiDim<4>
{
public:
    ParamRGBA(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamRGBA* clone() const { return new ParamRGBA(*this); }

    OfxRGBAColourD getDefault() const;

    void getValue(double& r, double& g, double& b, double& a) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, double& r, double& g, double& b, double& a) const OFX_EXCEPTION_SPEC;

    void setValue(const int& r, const int& g, const int& b, const int& a,
                  const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValue((double)r, (double)g, (double)b, (double)a, change);
    }
    void setValue(const double& r, const double& g, const double& b, const double& a,
                  const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValueAtTime(const OfxTime time, const int& r, const int& g, const int& b, const int& a,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, (double)r, (double)g, (double)b, (double)a, change);
    }
    void setValueAtTime(const OfxTime time, const double& r, const double& g, const double& b,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, r, g, b, 1.0, change);
    }
    void setValueAtTime(const OfxTime time, const int& r, const int& g, const int& b,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, (double)r, (double)g, (double)b, change);
    }
    void setValueAtTime(const OfxTime time, const double& r, const double& g, const double& b, const double& a,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
};
}
}
}

#endif
