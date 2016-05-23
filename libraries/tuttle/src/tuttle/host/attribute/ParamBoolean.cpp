#include "ParamBoolean.hpp"
#include "expression.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamBoolean::ParamBoolean(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                           const std::size_t index)
    : Param(effect)
    , ofx::attribute::OfxhParamBoolean(descriptor, name, effect.getParamSet(), index)
{
    _value = getDefault();
}

bool ParamBoolean::getDefault() const
{
    return static_cast<bool>(getProperties().getIntProperty(kOfxParamPropDefault, this->_index) != 0);
}

void ParamBoolean::getValue(bool& v) const OFX_EXCEPTION_SPEC
{
    v = _value;
}

void ParamBoolean::getValueAtTime(const OfxTime time, bool& v) const OFX_EXCEPTION_SPEC
{
    v = _value; ///< @todo: in time !
}

void ParamBoolean::setValue(const bool& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = v;
    paramChanged(change);
}

void ParamBoolean::setValueAtTime(const OfxTime time, const bool& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = v; ///< @todo: in time !
    paramChanged(change);
}

void ParamBoolean::setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _value = extractValueFromExpression<bool>(value);
    this->paramChanged(change);
}

void ParamBoolean::copy(const ParamBoolean& p) OFX_EXCEPTION_SPEC
{
    _value = p._value;
    //	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamBoolean::copy(const OfxhParam& p) OFX_EXCEPTION_SPEC
{
    const ParamBoolean& param = dynamic_cast<const ParamBoolean&>(p);

    copy(param);
}
}
}
}
