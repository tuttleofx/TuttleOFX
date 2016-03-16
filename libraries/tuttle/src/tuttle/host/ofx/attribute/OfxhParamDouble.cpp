#include "OfxhParamDouble.hpp"

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
void OfxhParamDouble::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    double* value = va_arg(arg, double*);

    getValue(*value);
}

/**
 * implementation of var args function
 */
void OfxhParamDouble::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    double* value = va_arg(arg, double*);

    getValueAtTime(time, *value);
}

/**
 * implementation of var args function
 */
void OfxhParamDouble::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    double value = va_arg(arg, double);

    setValue(value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamDouble::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    double value = va_arg(arg, double);

    setValueAtTime(time, value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamDouble::deriveV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    double* value = va_arg(arg, double*);

    derive(time, *value);
}

/**
 * implementation of var args function
 */
void OfxhParamDouble::integrateV(const OfxTime time1, const OfxTime time2, va_list arg) const OFX_EXCEPTION_SPEC
{
    double* value = va_arg(arg, double*);

    integrate(time1, time2, *value);
}
}
}
}
}
