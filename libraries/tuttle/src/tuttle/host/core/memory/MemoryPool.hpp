#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_

#include "IMemoryPool.hpp"
#include <climits>
#include <list>
#include <sstream>
#include <numeric>
#include <functional>

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

	virtual IPoolDataPtr allocate( const size_t size ) throw( std::bad_alloc, std::length_error );

	virtual void referenced( PoolData* );
	virtual void released( PoolData* );

	virtual size_t getUsedMemorySize() const;
	virtual size_t getAllocatedMemorySize() const;
	virtual size_t getMaxMemorySize() const;
	virtual size_t getAvailableMemorySize() const;
	virtual size_t getWastedMemorySize() const;

	virtual void clear( size_t size );
	virtual void clearOne();
	virtual void clearAll();

private:
	typedef std::list<PoolData*> DataList;
	DataList _dataUsed;
	DataList _dataUnused;
	const size_t _memoryAuthorized;
};

}
}
}

#endif

