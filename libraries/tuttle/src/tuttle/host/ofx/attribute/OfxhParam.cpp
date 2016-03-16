#include "OfxhParam.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhParamDescriptor.hpp"

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * make a parameter, with the given type and name
 */
OfxhParam::OfxhParam(const OfxhParamDescriptor& descriptor, const std::string& name, attribute::OfxhParamSet& setInstance)
    : attribute::OfxhAttribute(descriptor)
    , _paramSetInstance(&setInstance)
    , _parentInstance(NULL)
    , _avoidRecursion(false)
{
    // parameter has to be owned by paramSet
    // setInstance.referenceParam( name, this ); ///< @todo tuttle move this outside

    getEditableProperties().addNotifyHook(kOfxParamPropEnabled, this);
    getEditableProperties().addNotifyHook(kOfxParamPropSecret, this);
    getEditableProperties().addNotifyHook(kOfxPropLabel, this);
}

/**
 * the description of a plugin parameter
 */
OfxhParam::~OfxhParam()
{
}

void OfxhParam::paramChanged(const EChange change)
{
    _paramSetInstance->paramChanged(*this, change);
}

/**
 * callback which should set enabled state as appropriate
 */
void OfxhParam::setEnabled()
{
}

/**
 * callback which should set secret state as appropriate
 */
void OfxhParam::setSecret()
{
}

/**
 * callback which should update label
 */
void OfxhParam::setLabel()
{
}

/**
 * callback which should set
 */
void OfxhParam::setDisplayRange()
{
}

/**
 * get a value, implemented by instances to deconstruct var args
 */
void OfxhParam::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(
        OfxhException(kOfxStatErrUnsupported, std::string("ParamInstance getV failed (paramName:") + getName() + ")"));
}

/**
 * get a value, implemented by instances to deconstruct var args
 */
void OfxhParam::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported,
                                        std::string("ParamInstance getV at time failed (paramName:") + getName() + ")"));
}

/**
 * set a value, implemented by instances to deconstruct var args
 */
void OfxhParam::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(
        OfxhException(kOfxStatErrUnsupported, std::string("ParamInstance setV failed (paramName:") + getName() + ")"));
}

/**
 * key a value, implemented by instances to deconstruct var args
 */
void OfxhParam::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported,
                                        std::string("ParamInstance setV at time failed (paramName:") + getName() + ")"));
}

/**
 * derive a value, implemented by instances to deconstruct var args
 */
void OfxhParam::deriveV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported));
}

/**
 * integrate a value, implemented by instances to deconstruct var args
 */
void OfxhParam::integrateV(const OfxTime time1, const OfxTime time2, va_list arg) const OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported));
}

/**
 * overridden from Property::NotifyHook
 */
void OfxhParam::notify(const std::string& name, bool single, int num) OFX_EXCEPTION_SPEC
{
    if(name == kOfxPropLabel)
    {
        setLabel();
    }
    if(name == kOfxParamPropEnabled)
    {
        setEnabled();
    }
    if(name == kOfxParamPropSecret)
    {
        setSecret();
    }
    if(name == kOfxParamPropDisplayMin || name == kOfxParamPropDisplayMax)
    {
        setDisplayRange();
    }
}

/**
 * copy one parameter to another
 */
void OfxhParam::copy(const OfxhParam& instance, OfxTime offset) OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

/**
 * copy one parameter to another, with a range
 */
void OfxhParam::copy(const OfxhParam& instance, OfxTime offset, OfxRangeD range) OFX_EXCEPTION_SPEC
{
    BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature));
}

void OfxhParam::setParentInstance(OfxhParam* instance)
{
    _parentInstance = instance;
}

OfxhParam* OfxhParam::getParentInstance()
{
    return _parentInstance;
}

std::ostream& operator<<(std::ostream& os, const OfxhParam& v)
{
    os << "Param {" << std::endl;
    os << v.getProperties();
    os << "}" << std::endl;
    return os;
}
}
}
}
}
