#include "ParamChoice.hpp"
#include "expression.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamChoice::ParamChoice(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ofx::attribute::OfxhParamChoice(descriptor, name, effect.getParamSet())
{
    _value = getDefault();
}

int ParamChoice::getDefault() const
{
    return getProperties().getIntProperty(kOfxParamPropDefault);
}

void ParamChoice::getValue(int& v) const OFX_EXCEPTION_SPEC
{
    v = _value;
}

void ParamChoice::getValueAtTime(const OfxTime time, int& v) const OFX_EXCEPTION_SPEC
{
    v = _value; ///< @todo: in time !
}

void ParamChoice::setValue(const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    const std::size_t nbKeys = this->getChoiceKeys().size();
    if(v < 0 || static_cast<std::size_t>(v) > nbKeys)
    {
        BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Choice index is out of range. Index is " + v +
                                                        " for " + nbKeys + " values.");
    }
    _value = v;
    paramChanged(change);
}

void ParamChoice::setValueAtTime(const OfxTime time, const int& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    const std::size_t nbKeys = this->getChoiceKeys().size();
    if(v < 0 || static_cast<std::size_t>(v) > nbKeys)
    {
        BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Choice index is out of range. Index is " + v +
                                                        " for " + nbKeys + " values.");
    }
    _value = v; ///< @todo: in time !
    paramChanged(change);
}

void ParamChoice::setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    try
    {
        const std::string key = extractValueFromExpression<std::string>(value);
        OfxhParamChoice::setValue(key, change);
        return;
    }
    catch(...)
    {
        try
        {
            const int id = extractValueFromExpression<int>(value);
            this->setValue(id, change);
            return;
        }
        catch(...)
        {
        }
        throw;
    }
}

void ParamChoice::copy(const ParamChoice& p) OFX_EXCEPTION_SPEC
{
    _value = p._value;
    //	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamChoice::copy(const OfxhParam& p) OFX_EXCEPTION_SPEC
{
    const ParamChoice& param = dynamic_cast<const ParamChoice&>(p);

    copy(param);
}
}
}
}
