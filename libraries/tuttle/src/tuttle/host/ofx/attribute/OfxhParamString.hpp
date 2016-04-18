#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSTRING_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSTRING_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhParamString : public OfxhParam, public OfxhKeyframeParam
{
    mutable std::string _returnValue; ///< location to hold temporary return value. Should delegate this to implementation!!!

public:
    typedef std::string BaseType;
    OfxhParamString(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance)
        : OfxhParam(descriptor, name, setInstance)
    {
    }

    const std::string& getStringMode() const;

    virtual void getValue(std::string&) const OFX_EXCEPTION_SPEC = 0;
    virtual void getValueAtTime(const OfxTime time, std::string&) const OFX_EXCEPTION_SPEC = 0;
    virtual void setValue(const std::string&, const EChange change) OFX_EXCEPTION_SPEC = 0;
    virtual void setValueAtTime(const OfxTime time, const std::string&, const EChange change) OFX_EXCEPTION_SPEC = 0;

#ifndef SWIG
    /// implementation of var args function
    virtual void getV(va_list arg) const OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC;
#endif

    bool paramTypeHasData() const { return true; }

    std::size_t getHashAtTime(const OfxTime time) const;

    std::ostream& displayValues(std::ostream& os) const
    {
        os << getStringValue();
        return os;
    }
};
}
}
}
}

#endif
