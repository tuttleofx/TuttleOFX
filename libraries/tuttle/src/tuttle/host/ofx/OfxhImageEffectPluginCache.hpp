#ifndef _OFXH_IMAGEEFFECTPLUGINCACHE_HPP_
#define _OFXH_IMAGEEFFECTPLUGINCACHE_HPP_

#include "OfxhImageEffectHost.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhMajorPlugin.hpp"
#include "OfxhHost.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

/// implementation of the specific Image Effect handler API cache.
class OfxhImageEffectPluginCache : public APICache::OfxhPluginAPICacheI
{
public:
    typedef OfxhImageEffectPluginCache This;
    typedef std::map<std::string, OfxhImageEffectPlugin*> MapPluginsByID;

private:
    /// all plugins
    std::vector<OfxhImageEffectPlugin*> _plugins;

    /// latest version of each plugin by ID
    MapPluginsByID _pluginsByID;

    /// latest minor version of each plugin by (ID,major)
    std::map<OfxhMajorPlugin, OfxhImageEffectPlugin*> _pluginsByIDMajor;

    /// pointer to our image effect host
    OfxhImageEffectHost* _host;

public:
    explicit OfxhImageEffectPluginCache(OfxhImageEffectHost& host);
    ~OfxhImageEffectPluginCache();

    /// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
    /// pick the highest found version.
    OfxhImageEffectPlugin* getPluginById(const std::string& id, int vermaj = -1, int vermin = -1);
    const OfxhImageEffectPlugin* getPluginById(const std::string& id, int vermaj = -1, int vermin = -1) const
    {
        return const_cast<This&>(*this).getPluginById(id, vermaj, vermin);
    }

    /// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
    /// pick the highest found version.
    OfxhImageEffectPlugin* getPluginByLabel(const std::string& label, int vermaj = -1, int vermin = -1);
    const OfxhImageEffectPlugin* getPluginByLabel(const std::string& label, int vermaj = -1, int vermin = -1) const
    {
        return const_cast<This&>(*this).getPluginByLabel(label, vermaj, vermin);
    }

    OfxhImageEffectHost& getHost() { return *_host; }

    const std::vector<OfxhImageEffectPlugin*>& getPlugins() const;

    const MapPluginsByID& getPluginsByID() const;

    /// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
    void loadFromPlugin(OfxhPlugin& p);

    void confirmPlugin(OfxhPlugin& p);

    bool pluginSupported(const OfxhPlugin& p, std::string& reason) const;

#ifndef SWIG
    OfxhPlugin* newPlugin(OfxhPluginBinary& pb, int pi, OfxPlugin& pl);

    OfxhPlugin* newPlugin(OfxhPluginBinary& pb, int pi, const std::string& api, int apiVersion, const std::string& pluginId,
                          const std::string& rawId, int pluginMajorVersion, int pluginMinorVersion);
#endif

    friend std::ostream& operator<<(std::ostream& os, const This& g);
};
}
}
}
}

#endif
