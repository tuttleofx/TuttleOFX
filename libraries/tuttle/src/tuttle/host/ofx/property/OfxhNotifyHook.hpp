#ifndef _TUTTLE_HOST_OFX_PROPERTY_NOTIFYHOOK_HPP_
#define _TUTTLE_HOST_OFX_PROPERTY_NOTIFYHOOK_HPP_

#include <tuttle/host/ofx/OfxhException.hpp>
#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace property
{

/// Sits on a property and is called when the local property is being set.
/// It notify or notifyN is called whenever the plugin sets a property
/// Many of these can sit on a property, as various objects will need to know when a property
/// has been changed. On notification you should fetch properties with a 'raw' call, rather
/// than the standard calls, as you may be fetching through a getHook and you won't see
/// the local value that has been shoved into the property.
class OfxhNotifyHook
{
public:
    /// dtor
    virtual ~OfxhNotifyHook() {}

    /// override this to be notified when a property changes
    /// \arg name is the name of the property just set
    /// \arg singleValue is whether setProperty on a single index was call, otherwise N properties were set
    /// \arg indexOrN is the index if single value is true, or the count if singleValue is false
    virtual void notify(const std::string& name, bool singleValue, int indexOrN) OFX_EXCEPTION_SPEC = 0;
};
}
}
}
}

#endif
