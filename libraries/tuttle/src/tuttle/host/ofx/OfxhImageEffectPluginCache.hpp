
#ifndef _OFXH_IMAGEEFFECTPLUGINCACHE_HPP_
#define _OFXH_IMAGEEFFECTPLUGINCACHE_HPP_

#include "OfxhImageEffectHost.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhMajorPlugin.hpp"
#include "OfxhHost.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {


/// implementation of the specific Image Effect handler API cache.
class OfxhImageEffectPluginCache : public APICache::OfxhPluginAPICacheI
{
private:
	/// all plugins
	std::vector<OfxhImageEffectPlugin*> _plugins;

	/// latest version of each plugin by ID
	std::map<std::string, OfxhImageEffectPlugin*> _pluginsByID;

	/// latest minor version of each plugin by (ID,major)
	std::map<OfxhMajorPlugin, OfxhImageEffectPlugin*> _pluginsByIDMajor;

	/// pointer to our image effect host
	OfxhImageEffectHost* _host;

public:
	explicit OfxhImageEffectPluginCache( OfxhImageEffectHost& host );
	~OfxhImageEffectPluginCache();

#ifndef SWIG
	/// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
	/// pick the highest found version.
	OfxhImageEffectPlugin* getPluginById( const std::string& id, int vermaj = -1, int vermin = -1 );

	/// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
	/// pick the highest found version.
	OfxhImageEffectPlugin* getPluginByLabel( const std::string& label, int vermaj = -1, int vermin = -1 );

	OfxhImageEffectHost* getHost() { return _host; }

	const std::vector<OfxhImageEffectPlugin*>& getPlugins() const;

	const std::map<std::string, OfxhImageEffectPlugin*>& getPluginsByID() const;

	const std::map<OfxhMajorPlugin, OfxhImageEffectPlugin*>& getPluginsByIDMajor() const { return _pluginsByIDMajor; }

	/// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
	void loadFromPlugin( OfxhPlugin* p ) const;

	void confirmPlugin( OfxhPlugin* p );

	bool pluginSupported( OfxhPlugin* p, std::string& reason ) const;

	OfxhPlugin* newPlugin( OfxhPluginBinary* pb,
	                       int               pi,
	                       OfxPlugin*        pl );

	OfxhPlugin* newPlugin( OfxhPluginBinary*  pb,
	                       int                pi,
	                       const std::string& api,
	                       int                apiVersion,
	                       const std::string& pluginId,
	                       const std::string& rawId,
	                       int                pluginMajorVersion,
	                       int                pluginMinorVersion );
#endif
	void dumpToStdOut() const;
};

}
}
}
}

#endif

