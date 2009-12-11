#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_

#include "IMemoryCache.hpp"
#include "IMemoryPool.hpp"

#include <boost/ptr_container/ptr_map.hpp>
#include <map>

namespace tuttle {
namespace host {
namespace core {

class MemoryCache : public IMemoryCache
{
MemoryCache( const MemoryCache& pool );

public:
	MemoryCache() {}
	~MemoryCache() {}

private:
	struct Key
	{
		Key( const std::string& name, const double& time )
			: _pluginName( name ),
			_time( time )
		{}
		bool operator<( const Key& ) const;
		std::string _pluginName;
		double _time;
	};
	typedef std::map<Key, CACHE_ELEMENT> MAP;
	MAP _map;
	MAP::const_iterator getIteratorForValue( const CACHE_ELEMENT& ) const;
	MAP::iterator       getIteratorForValue( const CACHE_ELEMENT& );

public:
	virtual void               put( const std::string& pluginName, const double& time, CACHE_ELEMENT pData );
	virtual CACHE_ELEMENT       get( const std::string& pluginName, const double& time ) const;
	virtual std::size_t        size() const;
	virtual bool               empty() const;
	virtual bool               inCache( const CACHE_ELEMENT& ) const;
	virtual double             getTime( const CACHE_ELEMENT& ) const;
	virtual const std::string& getPluginName( const CACHE_ELEMENT& ) const;
	virtual bool               remove( const CACHE_ELEMENT& );
	virtual void               clearAll();
};

}
}
}

#endif

