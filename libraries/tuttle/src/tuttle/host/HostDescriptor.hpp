#ifndef _TUTTLE_HOST_DESCRIPTOR_HPP_
#define _TUTTLE_HOST_DESCRIPTOR_HPP_

#include <tuttle/host/ofx/OfxhImageEffectHost.hpp>
#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle
{
namespace host
{

/** a host combines several things...
 *    - a factory to create a new instance of your plugin
 *      - it also gets to filter some calls during in the
 *         API to check for validity and perform custom
 *         operations (eg: add extra properties).
 *     - it provides a description of the host application
 *       which is passed back to the plugin.
 *
 * @todo how to support multiple APIs...
 */
class Host : public tuttle::host::ofx::imageEffect::OfxhImageEffectHost
{
public:
    Host();

    /** Create a new instance of an image effect plug-in.
     *
     *  It is called by ImageEffectPlugin::createInstance which the
     *  client code calls when it wants to make a new instance.
     *
     *  @arg clientData - the clientData passed into the ImageEffectPlugin::createInstance
     *  @arg plugin - the plugin being created
     *  @arg desc - the descriptor for that plugin
     *  @arg context - the context to be created in
     */
    tuttle::host::ImageEffectNode* newInstance(tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin,
                                               tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
                                               const std::string& context) const;
    /// Override this to create a descriptor, this makes the 'root' descriptor
    tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
    makeDescriptor(tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin) const;

    /// used to construct a context description, rootContext is the main context
    tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
    makeDescriptor(const tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& rootContext,
                   tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plug) const;

    /// used to construct populate the cache
    tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
    makeDescriptor(const std::string& bundlePath, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plug) const;
#ifndef SWIG
    /// vmessage
    OfxStatus vmessage(const char* type, const char* id, const char* format, va_list args) const;
#endif
};
}
}

#endif
