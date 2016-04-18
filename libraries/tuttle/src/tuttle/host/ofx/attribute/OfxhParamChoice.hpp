#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMCHOICE_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMCHOICE_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhParamChoice : public OfxhParam, public OfxhKeyframeParam
{
public:
    OfxhParamChoice(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance)
        : OfxhParam(descriptor, name, setInstance)
    {
    }
    virtual ~OfxhParamChoice() {}

    int getIndexFor(const std::string& key) const;
    const std::string& getChoiceKeyAt(const int index) const;

    const std::vector<std::string>& getChoiceKeys() const
    {
        return this->getProperties().fetchStringProperty(kOfxParamPropChoiceOption).getValues();
    }
    // Deriving implementatation needs to overide these
    virtual void getValue(int&) const OFX_EXCEPTION_SPEC = 0;
    virtual void getValueAtTime(const OfxTime time, int&) const OFX_EXCEPTION_SPEC = 0;
    virtual void setValue(const int&, const EChange change) OFX_EXCEPTION_SPEC = 0;
    virtual void setValueAtTime(const OfxTime time, const int&, const EChange change) OFX_EXCEPTION_SPEC = 0;

    void getValue(std::string& outKey) const OFX_EXCEPTION_SPEC;

    void getValueAtTime(const OfxTime time, std::string& outKey) const OFX_EXCEPTION_SPEC;

    void setValue(const std::string& key, const EChange change) OFX_EXCEPTION_SPEC { setValue(getIndexFor(key), change); }

    void setValueAtTime(const OfxTime time, const std::string& key, const EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, getIndexFor(key), change);
    }

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
};
}
}
}
}

#endif
