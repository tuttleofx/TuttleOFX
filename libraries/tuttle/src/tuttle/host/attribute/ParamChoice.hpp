#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCHOICE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCHOICE_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamChoice.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamChoice : public Param, public ofx::attribute::OfxhParamChoice
{
protected:
    int _value;

public:
    ParamChoice(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamChoice* clone() const { return new ParamChoice(*this); }

    int getDefault() const;

    void getValue(int&) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, int&) const OFX_EXCEPTION_SPEC;
    void setValue(const int&, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
    void setValueAtTime(const OfxTime time, const int&, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

#ifndef SWIG
    void copy(const ParamChoice& p) OFX_EXCEPTION_SPEC;
    void copy(const OfxhParam& p) OFX_EXCEPTION_SPEC;
#endif
};
}
}
}

#endif
