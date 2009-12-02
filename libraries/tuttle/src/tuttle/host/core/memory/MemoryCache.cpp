#include "MemoryCache.hpp"

namespace tuttle {
namespace host {
namespace core {

MemoryCache::MemoryCache( MemoryPool& pool )
: _pool( pool )
{
}

MemoryCache::~MemoryCache()
{
}

}
}
}
