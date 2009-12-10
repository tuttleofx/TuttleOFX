#include "MemoryPool.hpp"

namespace tuttle {
namespace host {
namespace core {

unsigned long MemoryPool::PoolData::_count = 0;

MemoryPool::MemoryPool()
	: _memoryAuthorized( 1000000000 )
{}

MemoryPool::~MemoryPool()
{}

SizeInteger MemoryPool::clear( SizeInteger size )
{}

SizeInteger MemoryPool::clearOne()
{}

SizeInteger MemoryPool::clearAll()
{
	_datasUnused.clear();
}

}
}
}
