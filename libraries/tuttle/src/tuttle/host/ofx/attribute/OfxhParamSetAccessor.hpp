#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSETACCESSOR_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSETACCESSOR_HPP_

#include <tuttle/host/ofx/property/OfxhSet.hpp>

#include <ofxParam.h>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// base class to the param set instance and param set descriptor
class OfxhParamSetAccessor
{
public:
    virtual ~OfxhParamSetAccessor() = 0;

    /// obtain a handle on this set for passing to the C api
    virtual OfxParamSetHandle getParamSetHandle() const = 0;

    /// get the property handle that lives with the set
    /// The plugin descriptor/instance that derives from
    /// this will provide this.
    virtual property::OfxhSet& getParamSetProps() = 0;
};
}
}
}
}

#endif
