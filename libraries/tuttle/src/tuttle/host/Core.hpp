#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include <tuttle/host/memory/IMemoryCache.hpp>
#include <tuttle/host/HostDescriptor.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>

#include <tuttle/common/patterns/Singleton.hpp>

namespace tuttle {
namespace host {

class Core : public Singleton<Core>
{
public:
	typedef Core This;
	friend class Singleton<Core>;

private:
	Core();
	~Core();

	#ifndef SWIG

private:
	Host _host;
	ofx::imageEffect::OfxhImageEffectPluginCache _imageEffectPluginCache;
	ofx::OfxhPluginCache _pluginCache;
	memory::IMemoryPool& _memoryPool;
	memory::IMemoryCache& _memoryCache;

public:
	const ofx::OfxhPluginCache& getPluginCache() const { return _pluginCache; }
	const Host&                 getHost() const        { return _host; }
	#endif

public:
	const ofx::imageEffect::OfxhImageEffectPluginCache& getImageEffectPluginCache() const { return _imageEffectPluginCache; }
	#ifndef SWIG
	memory::IMemoryPool&        getMemoryPool()        { return _memoryPool; }
	const memory::IMemoryPool&  getMemoryPool() const  { return _memoryPool; }
	memory::IMemoryCache&       getMemoryCache()       { return _memoryCache; }
	const memory::IMemoryCache& getMemoryCache() const { return _memoryCache; }

public:
	ofx::imageEffect::OfxhImageEffectPlugin* getImageEffectPluginById( const std::string& id, int vermaj = -1, int vermin = -1 )
	{
		return _imageEffectPluginCache.getPluginById( id, vermaj, vermin );
	}

	#endif

public:
	void preload();

	friend std::ostream& operator<<( std::ostream& os, const This& v );

	#ifdef SWIG
	%extend
	{
		const ofx::OfxhPlugin& __getitem__( const std::string& name ) const
		{
			return *self->getPluginCache().getPluginById( name );
		}

		std::string __str__() const
		{
			std::stringstream s;

			s << *self;
			return s.str();
		}

	}
	#endif

};

}
}

#endif

