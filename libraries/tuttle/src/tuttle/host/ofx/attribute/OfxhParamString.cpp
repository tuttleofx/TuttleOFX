#include "OfxhParamString.hpp"

#include <boost/functional/hash.hpp>
#include <boost/filesystem/operations.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

const std::string& OfxhParamString::getStringMode() const
{
    return getProperties().getStringProperty(kOfxParamPropStringMode);
}

void OfxhParamString::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    const char** value = va_arg(arg, const char**);

    this->getValue(
        _returnValue); /// @todo tuttle: "I so don't like this, temp storage should be delegated to the implementation"

    *value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    const char** value = va_arg(arg, const char**);

    this->getValueAtTime(time, _returnValue); // I so don't like this, temp storage should be delegated to the implementation

    *value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    char* value = va_arg(arg, char*);

    this->setValue(value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    char* value = va_arg(arg, char*);

    this->setValueAtTime(time, value, change);
}

std::size_t OfxhParamString::getHashAtTime(const OfxTime time) const
{
    std::string value;
    getValueAtTime(time, value);
    std::size_t seed = boost::hash_value(value);
    if(getStringMode() == kOfxParamStringIsFilePath)
    {
        if(boost::filesystem::exists(value))
        {
            boost::hash_combine(seed, boost::filesystem::last_write_time(value));
        }
    }
    return seed;
}
}
}
}
}
