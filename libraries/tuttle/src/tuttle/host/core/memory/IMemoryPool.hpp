#ifndef _TUTTLE_HOST_CORE_IMEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_IMEMORYPOOL_HPP_

#include <cstddef>

namespace tuttle {
namespace host {
namespace core {

class IMemoryPool
{
public:
	virtual std::size_t getMemorySizeUsed() const       = 0;
	virtual std::size_t getMemorySizeAllocated() const  = 0;
	virtual std::size_t getMemorySizeAuthorized() const = 0;
	virtual void        clear( std::size_t size )       = 0;
	virtual void        clearOne()                      = 0;
	virtual void        clearAll()                      = 0;
};

}
}
}

#endif
