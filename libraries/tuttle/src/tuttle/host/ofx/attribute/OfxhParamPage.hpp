#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMPAGE_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMPAGE_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <map>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhParamPage : public OfxhParam
{
public:
    OfxhParamPage(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance)
        : OfxhParam(descriptor, name, setInstance)
    {
    }
    const std::map<int, OfxhParam*>& getChildren() const;

protected:
    mutable std::map<int, OfxhParam*> _children; // if set in a notify hook, this need not be mutable

    // Page params don't have values; just ignore set/get
    virtual void getValue(double&) const OFX_EXCEPTION_SPEC{};
    virtual void getValueAtTime(const OfxTime time, double&) const OFX_EXCEPTION_SPEC{};
    virtual void setValue(const double&, const EChange change) OFX_EXCEPTION_SPEC{};
    virtual void setValueAtTime(const OfxTime time, const double&, const EChange change) OFX_EXCEPTION_SPEC{};
    virtual void setValue(const int& v, const EChange change) OFX_EXCEPTION_SPEC{};
    virtual void setValueAtTime(const OfxTime time, const int& v, const EChange change) OFX_EXCEPTION_SPEC{};
    virtual void setValueFromExpression(const std::string& value, const EChange change) OFX_EXCEPTION_SPEC {}
    inline void setValueFromExpression(const std::string& value) OFX_EXCEPTION_SPEC
    {
        setValueFromExpression(value, eChangeUserEdited);
    }

    bool paramTypeHasData() const { return false; }

    std::size_t getHashAtTime(const OfxTime time) const { return 0; }
};
}
}
}
}

#endif
