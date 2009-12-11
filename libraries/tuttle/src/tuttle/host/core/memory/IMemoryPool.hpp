#ifndef _TUTTLE_HOST_CORE_IMEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_IMEMORYPOOL_HPP_

#include <cstddef>

namespace tuttle {
namespace host {
namespace core {

class IMemoryPool
{
public:
	virtual size_t getUsedMemorySize() const       = 0;
	virtual size_t getAllocatedMemorySize() const       = 0;
	virtual size_t getAvailableMemorySize() const = 0;
	virtual size_t getWastedMemorySize() const = 0;
	virtual size_t getMaxMemorySize() const  = 0;
	virtual void        clear( size_t size )       = 0;
	virtual void        clearOne()                      = 0;
	virtual void        clearAll()                      = 0;
};

}
}
}

#endif
