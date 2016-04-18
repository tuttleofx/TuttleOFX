#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGB_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGB_HPP_

#include "Param.hpp"

#include "ParamDoubleMultiDim.hpp"

#include <ofxPixels.h>

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamRGB : public Param, public ParamDoubleMultiDim<3>
{
public:
    ParamRGB(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamRGB* clone() const { return new ParamRGB(*this); }

    OfxRGBColourD getDefault() const;
    void getValue(double& r, double& g, double& b) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, double& r, double& g, double& b) const OFX_EXCEPTION_SPEC;
    void setValue(const double& r, const double& g, const double& b,
                  const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
    void setValueAtTime(const OfxTime time, const double& r, const double& g, const double& b,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValue(const int& r, const int& g, const int& b, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValue((double)r, (double)g, (double)b, change);
    }
    void setValueAtTime(const OfxTime time, const int& r, const int& g, const int& b,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, (double)r, (double)g, (double)b, change);
    }
};
}
}
}

#endif
