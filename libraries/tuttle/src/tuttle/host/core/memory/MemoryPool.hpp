#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_

#include "IMemoryPool.hpp"

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/unordered_set.hpp>

#include <map>
#include <list>
#include <sstream>
#include <numeric>
#include <functional>
#include <climits>

namespace tuttle {
namespace host {
namespace core {

class PoolData; ///< forward declaration
class IPool
{
public:
	virtual ~IPool() = 0;
	virtual void referenced( PoolData* ) = 0;
	virtual void released( PoolData* )   = 0;
};

/**
 * @todo tuttle: virtual destructor or nothing in virtual
 */
class MemoryPool : public IMemoryPool,
	public IPool
{
public:
	MemoryPool( const size_t maxSize = ULONG_MAX );
	~MemoryPool();

	IPoolDataPtr allocate( const size_t size ) throw( std::bad_alloc, std::length_error );

	void referenced( PoolData* );
	void released( PoolData* );

	size_t getUsedMemorySize() const;
	size_t getAllocatedMemorySize() const;
	size_t getMaxMemorySize() const;
	size_t getAvailableMemorySize() const;
	size_t getWastedMemorySize() const;

	void clear( size_t size );
	void clearOne();
	void clearAll();

private:
	typedef boost::unordered_set<PoolData*> DataList;
	boost::ptr_list<PoolData> _allDatas; // the owner
	std::map<char*,PoolData*> _dataMap; // the owner
	DataList _dataUsed;
	DataList _dataUnused;
	const size_t _memoryAuthorized;
};

}
}
}

#endif

