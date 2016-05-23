#include "OfxhGetHook.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace property
{

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
int OfxhGetHook::getProperty<OfxhIntValue>(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
    return getIntProperty(name, index);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
double OfxhGetHook::getProperty<OfxhDoubleValue>(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
    return getDoubleProperty(name, index);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
void* OfxhGetHook::getProperty<OfxhPointerValue>(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
    return getPointerProperty(name, index);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
const std::string& OfxhGetHook::getProperty<OfxhStringValue>(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
    return getStringProperty(name, index);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template <>
void OfxhGetHook::getPropertyN<OfxhIntValue>(const std::string& name, int* values, int count) const OFX_EXCEPTION_SPEC
{
    getIntPropertyN(name, values, count);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
void OfxhGetHook::getPropertyN<OfxhDoubleValue>(const std::string& name, double* values, int count) const OFX_EXCEPTION_SPEC
{
    getDoublePropertyN(name, values, count);
}

/// this does some magic so that it calls get string/int/double/pointer appropriately
template <>
void OfxhGetHook::getPropertyN<OfxhPointerValue>(const std::string& name, void** values, int count) const OFX_EXCEPTION_SPEC
{
    getPointerPropertyN(name, values, count);
}

/// override this to get a single value at the given index.
const std::string& OfxhGetHook::getStringProperty(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getStringProperty!!!! " << std::endl;
#endif
    return OfxhStringValue::kEmpty;
}

/// override this to fetch a single value at the given index.
int OfxhGetHook::getIntProperty(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getIntProperty!!!! " << std::endl;
#endif
    return 0;
}

/// override this to fetch a single value at the given index.
double OfxhGetHook::getDoubleProperty(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getDoubleProperty!!!! " << std::endl;
#endif
    return 0;
}

/// override this to fetch a single value at the given index.
void* OfxhGetHook::getPointerProperty(const std::string& name, int index) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getPointerProperty!!!! " << std::endl;
#endif
    return NULL;
}

/// override this to fetch a multiple values in a multi-dimension property
void OfxhGetHook::getDoublePropertyN(const std::string& name, double* values, int count) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getDoublePropertyN!!!! " << std::endl;
#endif
    memset(values, 0, sizeof(double) * count);
}

/// override this to fetch a multiple values in a multi-dimension property
void OfxhGetHook::getIntPropertyN(const std::string& name, int* values, int count) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getIntPropertyN!!!! " << std::endl;
#endif
    memset(values, 0, sizeof(int) * count);
}

/// override this to fetch a multiple values in a multi-dimension property
void OfxhGetHook::getPointerPropertyN(const std::string& name, void** values, int count) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getPointerPropertyN!!!! " << std::endl;
#endif
    memset(values, 0, sizeof(void*) * count);
}

/// override this to fetch the dimension size.
size_t OfxhGetHook::getDimension(const std::string& name) const OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::getDimension!!!! " << std::endl;
#endif
    return 0;
}

/// override this to handle a reset().
void OfxhGetHook::reset(const std::string& name) OFX_EXCEPTION_SPEC
{
#ifdef DEBUG_PROPERTIES
    std::cout << "Calling un-overriden GetHook::reset!!!! " << std::endl;
#endif
}
}
}
}
}
