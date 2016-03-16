#include "OfxhParamInteger.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

void OfxhParamInteger::derive(const OfxTime time, int&) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported));
}

void OfxhParamInteger::integrate(const OfxTime time1, const OfxTime time2, int&) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported));
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    this->getValue(*value);
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    this->getValueAtTime(time, *value);
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    int value = va_arg(arg, int);

    this->setValue(value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    int value = va_arg(arg, int);

    this->setValueAtTime(time, value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::deriveV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    this->derive(time, *value);
}

/**
 * implementation of var args function
 */
void OfxhParamInteger::integrateV(const OfxTime time1, const OfxTime time2, va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    this->integrate(time1, time2, *value);
}
}
}
}
}
