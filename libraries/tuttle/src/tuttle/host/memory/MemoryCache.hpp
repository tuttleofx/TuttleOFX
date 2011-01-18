#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_

#include "IMemoryCache.hpp"
#include "IMemoryPool.hpp"

//#include <boost/ptr_container/ptr_map.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>
//#include <map>

namespace tuttle {
namespace host {
namespace memory {

class MemoryCache : public IMemoryCache
{
typedef MemoryCache This;

public:
	MemoryCache( const MemoryCache& cache ) : _map(cache._map) {}
	MemoryCache() {}
	~MemoryCache() {}

	MemoryCache& operator=( const MemoryCache& cache );

private:
	typedef boost::unordered_map<Key, CACHE_ELEMENT, KeyHash> MAP;
	//	typedef std::map<Key, CACHE_ELEMENT> MAP;
	MAP _map;
	mutable boost::mutex _mutexMap;  ///< Mutex for cache data map.

	MAP::const_iterator getIteratorForValue( const CACHE_ELEMENT& ) const;
	MAP::iterator       getIteratorForValue( const CACHE_ELEMENT& );

public:
	void               put( const std::string& identifier, const double time, CACHE_ELEMENT pData );
	CACHE_ELEMENT      get( const std::string& identifier, const double time ) const;
	std::size_t        size() const;
	bool               empty() const;
	bool               inCache( const CACHE_ELEMENT& ) const;
	double             getTime( const CACHE_ELEMENT& ) const;
	const std::string& getPluginName( const CACHE_ELEMENT& ) const;
	bool               remove( const CACHE_ELEMENT& );
	void               clearUnused();
	void               clearAll();
	std::ostream& outputStream( std::ostream& os ) const
	{
		os << *this;
		return os;
	}
	friend std::ostream& operator<<( std::ostream& os, const MemoryCache& v );
};

}
}
}

#endif

