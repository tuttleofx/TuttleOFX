#include "OfxhPlugin.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{

OfxhPlugin::~OfxhPlugin()
{
}

bool OfxhPlugin::operator==(const This& other) const
{
    // don't compare _binary
    if(OfxhPluginDesc::operator!=(other) || _index != other._index)
        return false;
    return true;
}
}
}
}
