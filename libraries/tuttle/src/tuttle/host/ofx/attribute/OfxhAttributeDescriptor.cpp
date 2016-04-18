#include "OfxhAttributeDescriptor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// properties common to the desciptor and instance
/// the desc and set them, the instance cannot
static const property::OfxhPropSpec attributeDescriptorStuffs[] = {
    {kOfxPropName, property::ePropTypeString, 1, true, "SET_ME_ON_CONSTRUCTION"},
    {kOfxPropLabel, property::ePropTypeString, 1, false, ""},
    {kOfxPropShortLabel, property::ePropTypeString, 1, false, ""},
    {kOfxPropLongLabel, property::ePropTypeString, 1, false, ""},
    {0},
};

OfxhAttributeDescriptor::OfxhAttributeDescriptor()
    : _properties(property::OfxhSet())
{
    getEditableProperties().addProperties(attributeDescriptorStuffs);
}

OfxhAttributeDescriptor::OfxhAttributeDescriptor(const property::OfxhSet& properties)
    : _properties(properties)
{
    getEditableProperties().addProperties(attributeDescriptorStuffs);
}

OfxhAttributeDescriptor::~OfxhAttributeDescriptor()
{
}
}
}
}
}
