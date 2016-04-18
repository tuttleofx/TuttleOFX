#include "OfxhParamBoolean.hpp"

#include <boost/functional/hash.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * implementation of var args function
 */
void OfxhParamBoolean::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    bool v;

    getValue(v);

    int* value = va_arg(arg, int*);
    *value = v;
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    bool v;

    getValueAtTime(time, v);

    int* value = va_arg(arg, int*);
    *value = v;
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    bool value = va_arg(arg, int) != 0;

    setValue(value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    bool value = va_arg(arg, int) != 0;

    setValueAtTime(time, value, change);
}

std::size_t OfxhParamBoolean::getHashAtTime(const OfxTime time) const
{
    bool value = false;
    getValueAtTime(time, value);
    return boost::hash_value(value);
}
}
}
}
}
