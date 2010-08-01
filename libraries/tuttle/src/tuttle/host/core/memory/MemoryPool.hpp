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
	MemoryPool( const std::size_t maxSize = 0 );
	~MemoryPool();

	IPoolDataPtr allocate( const std::size_t size ) throw( std::bad_alloc, std::length_error );
	std::size_t updateMemoryAuthorizedWithRAM();

	void referenced( PoolData* );
	void released( PoolData* );

	std::size_t getUsedMemorySize() const;
	std::size_t getAllocatedMemorySize() const;
	std::size_t getMaxMemorySize() const;
	std::size_t getAvailableMemorySize() const;
	std::size_t getWastedMemorySize() const;

	void clear( std::size_t size );
	void clearOne();
	void clearAll();

private:
	typedef boost::unordered_set<PoolData*> DataList;
	boost::ptr_list<PoolData> _allDatas; // the owner
	std::map<char*,PoolData*> _dataMap; // the owner
	DataList _dataUsed;
	DataList _dataUnused;
	std::size_t _memoryAuthorized;
};

}
}
}

#endif

