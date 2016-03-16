#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_ATTRIBUTEACCESSOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_ATTRIBUTEACCESSOR_HPP_

#include <tuttle/host/ofx/property/OfxhSet.hpp>

#include <ofxCore.h>
#include <ofxAttribute.h>

#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhAttributeAccessor
{
public:
    OfxhAttributeAccessor();
    //	explicit Base( const Base& ); // auto
    virtual ~OfxhAttributeAccessor() = 0;

protected:
    virtual void setProperties(const property::OfxhSet& properties) = 0;

public:
    virtual const property::OfxhSet& getProperties() const = 0;
    virtual property::OfxhSet& getEditableProperties() = 0;

public:
    /// is the clip an output clip

    bool isOutput() const { return getName() == kOfxOutputAttributeName; }

    void setAllNames(const std::string& name)
    {
        setName(name);
        setLabel(name);
        setShortLabel(name);
        setLongLabel(name);
    }

    const std::string& getAttributeType() const { return getProperties().getStringProperty(kOfxPropType); }

    const std::string& getName() const { return getProperties().getStringProperty(kOfxPropName); }

    void setName(const std::string& name) { return getEditableProperties().setStringProperty(kOfxPropName, name); }

    /** name of the clip
     */
    const std::string& getShortLabel() const
    {
        const std::string& s = getProperties().getStringProperty(kOfxPropShortLabel);

        if(s == "")
        {
            return getLabel();
        }
        return s;
    }

    void setShortLabel(const std::string& label)
    {
        return getEditableProperties().setStringProperty(kOfxPropShortLabel, label);
    }

    /** name of the clip
     */
    const std::string& getLabel() const
    {
        const std::string& s = getProperties().getStringProperty(kOfxPropLabel);

        if(s == "")
        {
            return getName();
        }
        return s;
    }

    void setLabel(const std::string& label) { return getEditableProperties().setStringProperty(kOfxPropLabel, label); }

    /** name of the clip
     */
    const std::string& getLongLabel() const
    {
        const std::string& s = getProperties().getStringProperty(kOfxPropLongLabel);

        if(s == "")
        {
            return getLabel();
        }
        return s;
    }

    void setLongLabel(const std::string& label)
    {
        return getEditableProperties().setStringProperty(kOfxPropLongLabel, label);
    }

    /** get a handle on the properties of the clip descriptor for the C api
     */
    OfxPropertySetHandle getPropHandle() const { return getProperties().getHandle(); }
};
}
}
}
}

#endif
