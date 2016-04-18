#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCUSTOM_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCUSTOM_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamCustom.hpp>
#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

class ParamCustom : public Param, public ofx::attribute::OfxhParamCustom
{
protected:
    std::string _value; /// @todo link de parametres

public:
    ParamCustom(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor);
    ParamCustom* clone() const { return new ParamCustom(*this); }

    const std::string& getDefault() const;

    void getValue(std::string&) const OFX_EXCEPTION_SPEC;
    void getValueAtTime(const OfxTime time, std::string&) const OFX_EXCEPTION_SPEC;
    void setValue(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;
    void setValueAtTime(const OfxTime time, const std::string& value,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

    void setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC;

#ifndef SWIG
    void copy(const ParamCustom& p) OFX_EXCEPTION_SPEC;
    void copy(const OfxhParam& p) OFX_EXCEPTION_SPEC;
#endif
};
}
}
}

#endif
