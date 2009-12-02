#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP

#include "MemoryPool.hpp"

namespace tuttle {
namespace host {
namespace core {

class MemoryCache
{
public:
	MemoryCache( MemoryPool& pool );
	~MemoryCache();

private:
	MemoryPool& _pool;

public:
	bool release(){}
	bool purge(){}

};

}
}
}

#endif

