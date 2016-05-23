#include "ParamCustom.hpp"
#include "expression.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamCustom::ParamCustom(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ofx::attribute::OfxhParamCustom(descriptor, name, effect.getParamSet())
{
    _value = getDefault();
}

const std::string& ParamCustom::getDefault() const
{
    return getProperties().getStringProperty(kOfxParamPropDefault);
}

void ParamCustom::getValue(std::string& v) const OFX_EXCEPTION_SPEC
{
    v = _value;
}

void ParamCustom::getValueAtTime(const OfxTime time, std::string& v) const OFX_EXCEPTION_SPEC
{
    v = _value; ///< @todo: in time !
}

void ParamCustom::setValue(const std::string& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = v;
    this->paramChanged(change);
}

void ParamCustom::setValueAtTime(const OfxTime time, const std::string& v,
                                 const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = v; ///< @todo: in time !
    this->paramChanged(change);
}

void ParamCustom::setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = extractValueFromExpression<std::string>(value);
    this->paramChanged(change);
}

void ParamCustom::copy(const ParamCustom& p) OFX_EXCEPTION_SPEC
{
    _value = p._value;
    //	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamCustom::copy(const OfxhParam& p) OFX_EXCEPTION_SPEC
{
    const ParamCustom& param = dynamic_cast<const ParamCustom&>(p);

    copy(param);
}
}
}
}
