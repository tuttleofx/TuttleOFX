#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP

namespace tuttle {
namespace host {
namespace core {

class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();

public:
	bool release(){}
	bool purge(){}

};

}
}
}

#endif

