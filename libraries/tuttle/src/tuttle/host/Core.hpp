#ifndef _TUTTLE_HOST_CORE_HPP_
#define _TUTTLE_HOST_CORE_HPP_

#include "Preferences.hpp"

#include <tuttle/host/memory/IMemoryCache.hpp>
#include <tuttle/host/HostDescriptor.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>

#include <tuttle/common/patterns/Singleton.hpp>

#include <boost/preprocessor/stringize.hpp>


#define TUTTLE_HOST_VERSION_MAJOR 0
#define TUTTLE_HOST_VERSION_MINOR 5
#define TUTTLE_HOST_VERSION_MICRO 0

#define TUTTLE_HOST_VERSION_STR BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MAJOR) "." BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MINOR) "." BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MICRO)


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

private:
	Host _host;
	ofx::imageEffect::OfxhImageEffectPluginCache _imageEffectPluginCache;
	ofx::OfxhPluginCache _pluginCache;
	memory::IMemoryPool& _memoryPool;
	memory::IMemoryCache& _memoryCache;
	bool _isPreloaded;
	
	Preferences _preferences;

public:
	      ofx::OfxhPluginCache& getPluginCache()       { return _pluginCache; }
	const ofx::OfxhPluginCache& getPluginCache() const { return _pluginCache; }
	
	const std::list<ofx::OfxhPlugin*>& getPlugins() const { return getPluginCache().getPlugins(); }

	const Host&                 getHost() const        { return _host; }

	      Preferences& getPreferences()       { return _preferences; }
	const Preferences& getPreferences() const { return _preferences; }

public:
	const ofx::imageEffect::OfxhImageEffectPluginCache& getImageEffectPluginCache() const { return _imageEffectPluginCache; }

	memory::IMemoryPool&        getMemoryPool()        { return _memoryPool; }
	const memory::IMemoryPool&  getMemoryPool() const  { return _memoryPool; }
	memory::IMemoryCache&       getMemoryCache()       { return _memoryCache; }
	const memory::IMemoryCache& getMemoryCache() const { return _memoryCache; }

public:
	ofx::imageEffect::OfxhImageEffectPlugin* getImageEffectPluginById( const std::string& id, int vermaj = -1, int vermin = -1 )
	{
		return _imageEffectPluginCache.getPluginById( id, vermaj, vermin );
	}


public:
	void preload( const bool useCache = true );

	const ofx::OfxhPlugin& operator[]( const std::string& name ) const
	{
		return *( this->getPluginCache().getPluginById( name ) );
	}

	friend std::ostream& operator<<( std::ostream& os, const This& v );
};

inline Core& core() { return Core::instance(); }

}
}

#endif

