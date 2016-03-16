#include "OfxhAttribute.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhAttribute::OfxhAttribute(const property::OfxhSet& properties)
    : _properties(properties)
{
    BOOST_ASSERT(getAttributeType().c_str());
}

OfxhAttribute::OfxhAttribute(const OfxhAttributeDescriptor& desc)
    : _properties(desc.getProperties())
{
    BOOST_ASSERT(getAttributeType().c_str());
}

OfxhAttribute::~OfxhAttribute()
{
}
}
}
}
}
