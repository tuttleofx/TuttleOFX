#ifndef _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_

#include "IMemoryPool.hpp"
#include <string>

namespace tuttle {
namespace host {
namespace core {

class IMemoryCache
{
public:
	virtual void               put(const std::string& pluginName, const double &time, IPoolDataPtr pData) = 0;
	virtual IPoolDataPtr&      get(const std::string& pluginName, const double &time) const = 0;
	virtual std::size_t		   size() const = 0;
	virtual bool    		   empty() const = 0;
	virtual bool               inCache(const IPoolDataPtr &) const = 0;
	virtual double             getTime(const IPoolDataPtr &) const = 0;
	virtual const std::string& getPluginName(const IPoolDataPtr &) const = 0;
	virtual bool			   remove(const IPoolDataPtr &) = 0;
	virtual void			   clearAll() = 0;
};

}
}
}

#endif
