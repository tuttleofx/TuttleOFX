#include "OfxhKeyframeParam.hpp"

#include <boost/throw_exception.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

void OfxhKeyframeParam::getNumKeys(unsigned int& outNumKeys) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

void OfxhKeyframeParam::getKeyTime(const int nth, OfxTime& outTime) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

void OfxhKeyframeParam::getKeyIndex(const OfxTime time, const int direction, int& outIndex) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

void OfxhKeyframeParam::deleteKey(const OfxTime time) OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

void OfxhKeyframeParam::deleteAllKeys() OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}
}
}
}
}
