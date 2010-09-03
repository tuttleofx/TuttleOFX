#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_

#include "IMemoryCache.hpp"
#include "IMemoryPool.hpp"

//#include <boost/ptr_container/ptr_map.hpp>
#include <boost/unordered_map.hpp>
//#include <map>

namespace tuttle {
namespace host {
namespace memory {

class MemoryCache : public IMemoryCache
{
MemoryCache( const MemoryCache& pool );

public:
	MemoryCache() {}
	~MemoryCache() {}

private:
	struct Key
	{
		typedef Key This;
		Key( const std::string& identifier, const double& time )
			: _identifier( identifier )
			, _time( time )
		{}
		bool operator<( const This& ) const;
		bool operator==( const This& v ) const;
		std::size_t  getHash() const;

		std::string _identifier;
		double _time;
	};
	struct KeyHash : std::unary_function<Key, std::size_t>
	{
		std::size_t operator()( const Key& p ) const
		{
			return p.getHash();
		}

	};
	typedef boost::unordered_map<Key, CACHE_ELEMENT, KeyHash> MAP;
	//	typedef std::map<Key, CACHE_ELEMENT> MAP;
	MAP _map;
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
};

}
}
}

#endif

