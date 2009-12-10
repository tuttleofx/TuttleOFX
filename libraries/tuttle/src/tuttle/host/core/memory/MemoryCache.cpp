#include "MemoryCache.hpp"

namespace tuttle {
namespace host {
namespace core {

MemoryCache::CacheData::CacheData( PoolData& data )
	: _data( data ),
	_isUsed( false ),
	_willBeUsed( 0 ),
	_weight( 0 )
{
	/*_datas.push_back(&data);*/
}

MemoryCache::MemoryCache( MemoryPool& pool )
	: _pool( pool )
{}

MemoryCache::~MemoryCache()
{}

}
}
}
