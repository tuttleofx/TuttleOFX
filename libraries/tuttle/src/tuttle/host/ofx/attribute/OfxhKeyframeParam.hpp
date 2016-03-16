#ifndef _TUTTLE_HOST_OFX_PARAM_KEYFRAMEPARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_KEYFRAMEPARAM_HPP_

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhException.hpp>
#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhKeyframeParam
{
public:
    virtual ~OfxhKeyframeParam() {}

public:
    virtual void getNumKeys(unsigned int& outNumKeys) const OFX_EXCEPTION_SPEC;
    virtual void getKeyTime(const int nth, OfxTime& outTime) const OFX_EXCEPTION_SPEC;
    virtual void getKeyIndex(const OfxTime time, const int direction, int& outIndex) const OFX_EXCEPTION_SPEC;
    virtual void deleteKey(const OfxTime time) OFX_EXCEPTION_SPEC;
    virtual void deleteAllKeys() OFX_EXCEPTION_SPEC;
};
}
}
}
}

#endif
