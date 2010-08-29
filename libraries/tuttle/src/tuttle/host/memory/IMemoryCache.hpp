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

// configure the cache with this element
// CACHE_ELEMENT should be a smart_ptr of some kind
// pointer will be stored in a container so no auto_ptr is allowed

typedef ::boost::shared_ptr<tuttle::host::attribute::Image> CACHE_ELEMENT; ///< @todo temporary solution..
//typedef IPoolDataPtr CACHE_ELEMENT;

class IMemoryCache
{
public:
	virtual ~IMemoryCache() = 0;
	/// @todo tuttle: use key here, instead of (name, time)
	virtual void               put( const std::string& identifier, const double time, CACHE_ELEMENT pData ) = 0;
	virtual CACHE_ELEMENT      get( const std::string& identifier, const double time ) const                = 0;
	virtual std::size_t        size() const                                                                  = 0;
	virtual bool               empty() const                                                                 = 0;
	virtual bool               inCache( const CACHE_ELEMENT& ) const                                         = 0;
	virtual double             getTime( const CACHE_ELEMENT& ) const                                         = 0;
	virtual const std::string& getPluginName( const CACHE_ELEMENT& ) const                                   = 0;
	virtual bool               remove( const CACHE_ELEMENT& )                                                = 0;
	virtual void               clearUnused()                                                                 = 0;
	virtual void               clearAll()                                                                    = 0;
};

}
}
}

#endif
