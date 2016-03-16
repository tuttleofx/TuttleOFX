#include "OfxhParamSetDescriptor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * obtain a handle on this set for passing to the C api
 */
OfxhParamSetDescriptor::OfxhParamSetDescriptor()
{
}

OfxhParamSetDescriptor::~OfxhParamSetDescriptor()
{
}

void OfxhParamSetDescriptor::addParam(const std::string& name, OfxhParamDescriptor* p)
{
    _paramList.push_back(p);
    _paramMap[name] = p;
}

/**
 * define a param on this effect
 */
OfxhParamDescriptor* OfxhParamSetDescriptor::paramDefine(const char* paramType, const char* name)
{
    if(!isStandardType(paramType))
        BOOST_THROW_EXCEPTION(OfxhException(std::string("The param type '") + paramType + "' is not recognize, the param '" +
                                            name + "' can't be created."));

    OfxhParamDescriptor* desc = new OfxhParamDescriptor(paramType, name);
    desc->initStandardParamProps(paramType);
    addParam(name, desc);
    return desc;
}
}
}
}
}
