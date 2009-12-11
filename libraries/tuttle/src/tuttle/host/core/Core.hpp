#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include "EffectInstance.hpp"

#include <tuttle/host/core/memory/MemoryPool.hpp>
#include <tuttle/host/core/memory/MemoryCache.hpp>
#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

#include <tuttle/common/patterns/Singleton.hpp>

#include <stdexcept>

namespace tuttle {
namespace host {
namespace core {

class Core : public Singleton<Core>
{
public:
	friend class Singleton<Core>;

private:
	Core();
	~Core();

private:
	core::Host _host;
	ofx::imageEffect::ImageEffectPluginCache _imageEffectPluginCache;
	ofx::PluginCache _pluginCache;
	core::MemoryPool _memoryPool;
	core::MemoryCache _memoryCache;

public:
	const ofx::PluginCache&                         getPluginCache() const            { return _pluginCache; }
	const Host&                                     getHost() const                   { return _host; }
	const ofx::imageEffect::ImageEffectPluginCache& getImageEffectPluginCache() const { return _imageEffectPluginCache; }
	const core::MemoryPool&                         getMemoryPool() const             { return _memoryPool; }
	const core::MemoryCache&                        getMemoryCache() const            { return _memoryCache; }

public:
	ofx::imageEffect::ImageEffectPlugin* getImageEffectPluginById( const std::string& id, int vermaj = -1, int vermin = -1 )
	{
		return _imageEffectPluginCache.getPluginById( id, vermaj, vermin );
	}

public:
	void preload();

};

}
}
}

#endif

