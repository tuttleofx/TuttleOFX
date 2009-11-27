#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include "EffectInstance.hpp"

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

#include <tuttle/common/patterns/Singleton.hpp>

#include <stdexcept>

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
	core::Host _host;
	ofx::imageEffect::ImageEffectPluginCache _imageEffectPluginCache;
	ofx::PluginCache _pluginCache;

public:
	ofx::PluginCache& getPluginCache() { return _pluginCache; }
	Host& getHost() { return _host; }
	ofx::imageEffect::ImageEffectPluginCache& getImageEffectPluginCache() { return _imageEffectPluginCache; }

public:
	ofx::imageEffect::ImageEffectPlugin* getImageEffectPluginById(const std::string &id, int vermaj=-1, int vermin=-1)
	{
		return getImageEffectPluginCache().getPluginById( id, vermaj, vermin );
	}
	
public:
	void preload();

};

}
}
}

#endif

