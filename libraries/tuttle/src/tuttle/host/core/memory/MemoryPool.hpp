#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_

#include "IMemoryPool.hpp"
#include <climits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <numeric>
#include <functional>
#include <boost/smart_ptr/intrusive_ptr.hpp>

namespace tuttle {
namespace host {
namespace core {

class IUnknown{
public:
	virtual void addRef() = 0;
	virtual void release() = 0;
};

class IPoolData : public IUnknown {
public:
	virtual char* data() = 0;
	virtual const char* data() const = 0;
	virtual const size_t size() const = 0;
	virtual const size_t reservedSize() const = 0;
};

void intrusive_ptr_add_ref(IPoolData *pData);
void intrusive_ptr_release(IPoolData *pData);

typedef ::boost::intrusive_ptr<IPoolData> IPoolDataPtr;

class IPool; ///< forward declaration
class PoolData : public IPoolData
{
private:
	PoolData(); ///< No default Ctor
	PoolData(const PoolData&); ///< No copy Ctor
	friend class MemoryPool;

public:
	PoolData( IPool& pool, const size_t size ) :
		_pool(pool),
		_id(_count++),
		_reservedSize(size),
		_size( size ),
		_pData(new char[size]),
		_refCount(0)
	{}

	~PoolData()
	{
		delete [] _pData;
	}

public:
	bool operator==( const PoolData& other ) const
	{
		return _id == other._id;
	}

	virtual void addRef();
	virtual void release();

	virtual char*              data()       { return _pData; }
	virtual const char*        data() const { return _pData; }
	virtual const size_t  size() const { return _size; }
	virtual const size_t reservedSize() const { return _reservedSize; }

private:
	static size_t _count; ///< unique id generator
	IPool		&_pool; ///< ref to the owner pool
	const size_t _id; ///< unique id to identify one memory data
	const size_t _reservedSize; ///< memory allocated
	size_t _size; ///< memory requested
	char * const _pData; ///< own the data
	int _refCount; ///< counter on clients currently using this data
};

class IPool{
public:
	virtual void referenced(PoolData *) = 0;
	virtual void released(PoolData *) = 0;
};

class MemoryPool : public IMemoryPool, public IPool
{
public:
	MemoryPool( const size_t maxSize = ULONG_MAX );
	~MemoryPool();

	IPoolDataPtr allocate( const size_t size ) throw( std::bad_alloc, std::length_error );

	virtual void referenced(PoolData *);
	virtual void released(PoolData *);

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

