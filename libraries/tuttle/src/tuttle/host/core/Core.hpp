#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

#include <tuttle/common/patterns/Singleton.hpp>

namespace tuttle {
namespace host {
namespace core {

class Core : public Singleton<tuttle::host::core::Core>
{
public:
	friend class Singleton<Core>;
private:
	Core( );
	~Core( );

private:
	tuttle::host::core::Host _host;
	tuttle::host::ofx::imageEffect::ImageEffectPluginCache _imageEffectPluginCache;
	tuttle::host::ofx::PluginCache _pluginCache;

public:
	tuttle::host::core::Host& getHost() { return _host; }
	tuttle::host::ofx::imageEffect::ImageEffectPluginCache& getImageEffectPluginCache() { return _imageEffectPluginCache; }

public:
	ofx::imageEffect::ImageEffectPlugin *getImageEffectPluginById(const std::string &id, int vermaj=-1, int vermin=-1) { getImageEffectPluginCache().getPluginById( id, vermaj, vermin ); }
	
public:
	void preload();

};

}
}
}

#endif

