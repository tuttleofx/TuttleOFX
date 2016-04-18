#ifndef _TUTTLE_HOST_OFX_OBJECT_HPP_
#define _TUTTLE_HOST_OFX_OBJECT_HPP_

namespace tuttle
{
namespace host
{
namespace ofx
{

/**
 * @brief All openfx objects that can be send to the plugin need to inherit this class.
 * So all these objects can be seen as OfxhIObject (reinterpret_cast)
 * and then properly dynamic_cast to the desired class.
 */
class OfxhIObject
{
public:
    virtual ~OfxhIObject() = 0;
};
}
}
}

#endif
