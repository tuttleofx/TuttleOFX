#ifndef _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_

#include "IMemoryPool.hpp"

#include <boost/shared_ptr.hpp> ///< @todo temporary solution..
#include <string>

namespace tuttle {
namespace host {
namespace attribute {
class Image;
}
namespace memory {

/**
 * @brief configure the cache with this element
 * CACHE_ELEMENT should be a smart_ptr of some kind
 * pointer will be stored in a container so no auto_ptr is allowed
 */
typedef ::boost::shared_ptr<tuttle::host::attribute::Image> CACHE_ELEMENT; ///< @todo temporary solution..

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
	friend std::ostream& operator<<( std::ostream& os, const Key& v );
};
struct KeyHash : std::unary_function<Key, std::size_t>
{
	std::size_t operator()( const Key& p ) const
	{
		return p.getHash();
	}

};
//typedef IPoolDataPtr CACHE_ELEMENT;

class IMemoryCache
{
typedef IMemoryCache This;
public:
	virtual ~IMemoryCache() = 0;
	/// @todo tuttle: use key here, instead of (name, time)
	virtual void               put( const std::string& identifier, const double time, CACHE_ELEMENT pData ) = 0;
	virtual CACHE_ELEMENT      get( const std::string& identifier, const double time ) const                = 0;
	virtual std::size_t        size() const                                                                 = 0;
	virtual bool               empty() const                                                                = 0;
	virtual bool               inCache( const CACHE_ELEMENT& ) const                                        = 0;
	virtual double             getTime( const CACHE_ELEMENT& ) const                                        = 0;
	virtual const std::string& getPluginName( const CACHE_ELEMENT& ) const                                  = 0;
	virtual bool               remove( const CACHE_ELEMENT& )                                               = 0;
	virtual void               clearUnused()                                                                = 0;
	virtual void               clearAll()                                                                   = 0;
	virtual std::ostream&      outputStream( std::ostream& os ) const                                       = 0;
	friend std::ostream& operator<<( std::ostream& os, const This& v );
};

}
}
}

#endif
