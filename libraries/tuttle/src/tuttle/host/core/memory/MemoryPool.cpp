#include "MemoryPool.hpp"

namespace tuttle {
namespace host {
namespace core {

unsigned long MemoryPool::PoolData::_count = 0;

MemoryPool::MemoryPool( const std::size_t maxSize )
	: _memoryAuthorized( maxSize )
{}

MemoryPool::~MemoryPool()
{}

void MemoryPool::clear( std::size_t size )
{}

void MemoryPool::clearOne()
{}

void MemoryPool::clearAll()
{
	_datasUnused.clear();
}

}
}
}
