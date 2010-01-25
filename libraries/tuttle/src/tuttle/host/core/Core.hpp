#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include "ImageEffectNode.hpp"

#include <tuttle/host/core/memory/IMemoryCache.hpp>
#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>

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
	ofx::imageEffect::OfxhImageEffectPluginCache _imageEffectPluginCache;
	ofx::OfxhPluginCache _pluginCache;
	core::IMemoryPool& _memoryPool;
	core::IMemoryCache& _memoryCache;

public:
	const ofx::OfxhPluginCache&                         getPluginCache() const            { return _pluginCache; }
	const Host&                                         getHost() const                   { return _host; }
	const ofx::imageEffect::OfxhImageEffectPluginCache& getImageEffectPluginCache() const { return _imageEffectPluginCache; }
	core::IMemoryPool&                                  getMemoryPool()                   { return _memoryPool; }
	const core::IMemoryPool&                            getMemoryPool() const             { return _memoryPool; }
	core::IMemoryCache&                                 getMemoryCache()                  { return _memoryCache; }
	const core::IMemoryCache&                           getMemoryCache() const            { return _memoryCache; }

public:
	ofx::imageEffect::OfxhImageEffectPlugin* getImageEffectPluginById( const std::string& id, int vermaj = -1, int vermin = -1 )
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

