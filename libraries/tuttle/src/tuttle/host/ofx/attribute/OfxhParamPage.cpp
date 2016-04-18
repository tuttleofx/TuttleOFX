#include "OfxhParamPage.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

const std::map<int, OfxhParam*>& OfxhParamPage::getChildren() const
{
    // HACK!!!! this really should be done with a notify hook so we don't force
    // _children to be mutable
    if(_children.size() == 0)
    {
        int nChildren = getProperties().getDimension(kOfxParamPropPageChild);
        for(int i = 0; i < nChildren; i++)
        {
            std::string childName = getProperties().getStringProperty(kOfxParamPropPageChild, i);
            attribute::OfxhParam* child = &_paramSetInstance->getParam(childName);
            _children[i] = child;
        }
    }
    return _children;
}
}
}
}
}
