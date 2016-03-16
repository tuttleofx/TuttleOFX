#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMINTEGER_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMINTEGER_HPP_

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

class OfxhParamInteger : public OfxhParam, public OfxhKeyframeParam
{
protected:
    std::size_t _index;

public:
    typedef int BaseType;
    OfxhParamInteger(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance,
                     const std::size_t index = 0)
        : OfxhParam(descriptor, name, setInstance)
        , _index(index)
    {
        getEditableProperties().addNotifyHook(kOfxParamPropDisplayMin, this);
        getEditableProperties().addNotifyHook(kOfxParamPropDisplayMax, this);
    }

    // Deriving implementatation needs to overide these
    virtual void getValue(int&) const OFX_EXCEPTION_SPEC = 0;
    virtual void getValueAtTime(const OfxTime time, int&) const OFX_EXCEPTION_SPEC = 0;
    virtual void setValue(const int&, const EChange change) OFX_EXCEPTION_SPEC = 0;
    virtual void setValueAtTime(const OfxTime time, const int&, const EChange change) OFX_EXCEPTION_SPEC = 0;

    virtual void setValue(const bool& v, const EChange change) OFX_EXCEPTION_SPEC { setValue(int(v), change); }

    virtual void setValueAtTime(const OfxTime time, const bool& v, const EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, int(v), change);
    }

    // probably derived class does not need to implement, default is an approximation
    virtual void derive(const OfxTime time, int&) const OFX_EXCEPTION_SPEC;
    virtual void integrate(const OfxTime time1, const OfxTime time2, int&) const OFX_EXCEPTION_SPEC;

#ifndef SWIG
    /// implementation of var args function
    virtual void getV(va_list arg) const OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void deriveV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC;

    /// implementation of var args function
    virtual void integrateV(const OfxTime time1, const OfxTime time2, va_list arg) const OFX_EXCEPTION_SPEC;
#endif

    std::ostream& displayValues(std::ostream& os) const
    {
        os << getIntValue();
        return os;
    }
};
}
}
}
}

#endif
