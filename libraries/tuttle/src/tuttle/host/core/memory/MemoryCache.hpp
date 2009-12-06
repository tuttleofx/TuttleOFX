#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP

#include "MemoryPool.hpp"
#include <boost/ptr_container/ptr_list.hpp>
#include <cstring>

namespace tuttle {
namespace host {
namespace core {

class MemoryCache
{
public:
	class Data
	{
	public:
		typedef boost::ptr_list<MemoryPool::Data> PoolDataList;
	public:
		Data(MemoryPool::Data* data){ _datas.push_back(data); }
		~Data(){}
	public:
		PoolDataList _datas;
		int _willBeUsed; ///< we known that this data will be used x times in the future
		int _weight; ///< a coefficent representing the priority to keep this data (maybe the complexity to recalculate from scratch)
	};
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

