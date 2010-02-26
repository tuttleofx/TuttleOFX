#ifndef _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_
#define	_TUTTLE_PLUGIN_INTERACTOBJECT_HPP_

#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief A base class for all object to use in interact or overlay.
 * All sub classes needs to implement virtual functions from OFX::InteractI
 */
class InteractObject : public OFX::InteractI
{
};

}
}
}

#endif
