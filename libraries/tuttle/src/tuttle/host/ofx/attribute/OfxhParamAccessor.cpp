#include "OfxhParamAccessor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhParamAccessor::OfxhParamAccessor()
{
}

OfxhParamAccessor::~OfxhParamAccessor()
{
}

const std::string& OfxhParamAccessor::getParamType() const
{
    return getProperties().getStringProperty(kOfxParamPropType);
}

std::string OfxhParamAccessor::getParamTypeName() const
{
    static const std::string ofxPrefix = "OfxParamType";
    const std::string& type = getParamType();
    BOOST_ASSERT(type.size() > ofxPrefix.size());
    return type.substr(ofxPrefix.size());
}

const std::string& OfxhParamAccessor::getParentName() const
{
    return getProperties().getStringProperty(kOfxParamPropParent);
}

const std::string& OfxhParamAccessor::getScriptName() const
{
    return getProperties().getStringProperty(kOfxParamPropScriptName);
}

const std::string& OfxhParamAccessor::getHint() const
{
    return getProperties().getStringProperty(kOfxParamPropHint, 0);
}

const std::string& OfxhParamAccessor::getDoubleType() const
{
    return getProperties().getStringProperty(kOfxParamPropDoubleType, 0);
}

const std::string& OfxhParamAccessor::getCacheInvalidation() const
{
    return getProperties().getStringProperty(kOfxParamPropCacheInvalidation, 0);
}

bool OfxhParamAccessor::getEnabled() const
{
    return getProperties().getIntProperty(kOfxParamPropEnabled, 0) != 0;
}

bool OfxhParamAccessor::getSecret() const
{
    return getProperties().getIntProperty(kOfxParamPropSecret, 0) != 0;
}

bool OfxhParamAccessor::getEvaluateOnChange() const
{
    return getProperties().getIntProperty(kOfxParamPropEvaluateOnChange, 0) != 0;
}

bool OfxhParamAccessor::getCanUndo() const
{
    if(getProperties().hasProperty(kOfxParamPropCanUndo))
    {
        return getProperties().getIntProperty(kOfxParamPropCanUndo) != 0;
    }
    return false;
}

bool OfxhParamAccessor::getCanAnimate() const
{
    if(getProperties().hasProperty(kOfxParamPropAnimates))
    {
        return getProperties().getIntProperty(kOfxParamPropAnimates) != 0;
    }
    return false;
}
}
}
}
}
