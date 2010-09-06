#include "IMemoryCache.hpp"

namespace tuttle {
namespace host {
namespace memory {

IMemoryCache::~IMemoryCache() {}

std::ostream& operator<<( std::ostream& os, const IMemoryCache& v )
{
	return v.outputStream(os);
}

}
}
}

