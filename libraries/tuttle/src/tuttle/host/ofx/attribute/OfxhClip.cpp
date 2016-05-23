#include "OfxhClip.hpp"
#include "OfxhClipDescriptor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhClip::OfxhClip(const OfxhClipDescriptor& desc)
    : attribute::OfxhAttribute(desc)
{
    /// extra properties for the instance, these are fetched from the host
    /// via a get hook and some virtuals
    static const property::OfxhPropSpec clipInstanceStuffs[] = {
        {kOfxImageClipPropConnected, property::ePropTypeInt, 1, true, "0"}, {0},
    };

    // properties that are needed in an Instance but not a Descriptor
    getEditableProperties().addProperties(clipInstanceStuffs);
    initHook(clipInstanceStuffs);

    /// we are an instance, we need to reset the props to read only
    for(property::PropertyMap::iterator i = getEditableProperties().getMap().begin(),
                                        iEnd = getEditableProperties().getMap().end();
        i != iEnd; ++i)
    {
        i->second->setPluginReadOnly(false);
    }
}

OfxhClip::~OfxhClip()
{
}

void OfxhClip::initHook(const property::OfxhPropSpec* propSpec)
{
    // no more GetHook on clip for the moment...
    /*
       int i = 0;
       while( propSpec[i].name )
       {
        const Property::PropSpec& spec = propSpec[i];
        switch( spec.type )
        {
            case Property::eDouble :
            case Property::eString :
            case Property::eInt :
                getProperties().setGetHook( spec.name, this );
                break;
            default:
                break;
        }
     ++i;
       }
     */
}
}
}
}
}
