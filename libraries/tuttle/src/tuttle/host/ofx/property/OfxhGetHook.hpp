#ifndef _TUTTLE_HOST_OFX_PROPERTY_GETHOOK_HPP_
#define _TUTTLE_HOST_OFX_PROPERTY_GETHOOK_HPP_

#include "OfxhPropertyTemplate.hpp"
#include <tuttle/host/ofx/OfxhException.hpp>
#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace property
{

/// Sits on a property and can override the local property value when a value is being fetched
/// only one of these can be in any property (as the thing has only a single value).
/// We deliberately don't have a getStringPropertyN as it is somewhat more painfull and never
/// used in practice.
class OfxhGetHook
{
public:
    /// dtor
    virtual ~OfxhGetHook() {}

    /// We specialise this to do some magic so that it calls get string/int/double/pointer appropriately
    /// this is what is called by the propertytemplate code to fetch values out of a hook.
    template <class T>
    typename T::ReturnType getProperty(const std::string& name, int index = 0) const OFX_EXCEPTION_SPEC;

    /// We specialise this to do some magic so that it calls get int/double/pointer appropriately
    /// this is what is called by the propertytemplate code to fetch values out of a hook.
    template <class T>
    void getPropertyN(const std::string& name, typename T::APIType* values, int count) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a single value at the given index.
    virtual const std::string& getStringProperty(const std::string& name, int index = 0) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a single value at the given index.
    virtual int getIntProperty(const std::string& name, int index = 0) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a multiple values in a multi-dimension property
    virtual void getIntPropertyN(const std::string& name, int* values, int count) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a single value at the given index.
    virtual double getDoubleProperty(const std::string& name, int index = 0) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a multiple values in a multi-dimension property
    virtual void getDoublePropertyN(const std::string& name, double* values, int count) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a single value at the given index.
    virtual void* getPointerProperty(const std::string& name, int index = 0) const OFX_EXCEPTION_SPEC;

    /// override this to fetch a multiple values in a multi-dimension property
    virtual void getPointerPropertyN(const std::string& name, void** values, int count) const OFX_EXCEPTION_SPEC;

    /// override this to fetch the dimension size.
    virtual size_t getDimension(const std::string& name) const OFX_EXCEPTION_SPEC;

    /// override this to handle a reset().
    virtual void reset(const std::string& name) OFX_EXCEPTION_SPEC;
};
}
}
}
}

#endif
