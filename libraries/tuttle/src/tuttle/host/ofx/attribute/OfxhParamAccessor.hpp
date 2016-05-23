#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMACCESSOR_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMACCESSOR_HPP_

#include "OfxhAttributeAccessor.hpp"

#include <ofxParam.h>

#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// base class for all params
class OfxhParamAccessor : virtual public OfxhAttributeAccessor
{
public:
    OfxhParamAccessor();
    //        ParamAccessor( const ParamAccessor& other );
    virtual ~OfxhParamAccessor() = 0;

    /// grab a handle on the parameter for passing to the C API
    virtual OfxParamHandle getParamHandle() const = 0;

    virtual bool verifyMagic() { return true; }

    const std::string& getParamType() const;
    std::string getParamTypeName() const;

    const std::string& getParentName() const;

    /// @todo tuttle : common to all attributes
    const std::string& getScriptName() const;

    /// @todo tuttle : common to all attributes
    const std::string& getHint() const;

    const std::string& getDoubleType() const;

    const std::string& getCacheInvalidation() const;

    bool getEnabled() const;

    bool getCanUndo() const;

    bool getSecret() const;

    bool getEvaluateOnChange() const;

    bool getCanAnimate() const;
};
}
}
}
}

#endif
