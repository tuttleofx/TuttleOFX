#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP_

#include "IMemoryPool.hpp"
#include <boost/ptr_container/ptr_list.hpp>
#include <vector>
#include <numeric>
#include <cstring>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <climits>

namespace tuttle {
namespace host {
namespace core {

class MemoryPool : public IMemoryPool
{
public:
	class PoolData
	{
	static unsigned long _count;

	public:
		PoolData( const std::size_t size ) : _size( size )
		{
			_id   = _count++;
			_data = new char[size];
		}

		~PoolData()
		{
			delete [] _data;
		}

	public:
		bool operator==( const PoolData& other ) const
		{
			return _id == other._id;
		}

		char*              getData()       { return _data; }
		const char*        getData() const { return _data; }
		const std::size_t& getSize() const { return _size; }

	private:
		unsigned long _id; ///< unique id to identify one memory data
		char* _data; ///< own the data
		std::size_t _size; ///< data size
		//		bool _used; ///< is this data currently used
	};
	typedef boost::ptr_list<PoolData> DataList;
	DataList _datasUsed;
	DataList _datasUnused;
	const std::size_t _memoryAuthorized;

public:
	MemoryPool( const std::size_t maxSize = ULONG_MAX );
	~MemoryPool();

public:
	PoolData& allocate( std::size_t size ) throw( std::bad_alloc, std::length_error )
	{
		if( size > getMemorySizeAvailable() )
		{
			std::stringstream s;
			s << "MemoryPool can't allocate size:" << size << " because memorySizeAvailable=" << getMemorySizeAvailable();
			throw std::length_error( s.str() );
		}
		PoolData* data = new PoolData( size );
		_datasUsed.push_front( data );
		return *data;
	}

	void desallocate( PoolData& data )
	{
		DataList::iterator it = std::find( _datasUsed.begin(), _datasUsed.end(), data );

		if( it != _datasUsed.end() )
		{
			_datasUsed.erase( it );
		}
		else
		{
			it = std::find( _datasUnused.begin(), _datasUnused.end(), data );
			if( it != _datasUnused.end() )
			{
				_datasUnused.erase( it );
			}
			else
			{
				throw( std::logic_error( "Can't desallocate from the pool, data not found !" ) );
			}
		}
	}

	void setUsed( PoolData& data )
	{
		DataList::iterator it = std::find( _datasUsed.begin(), _datasUsed.end(), data );

		if( it != _datasUsed.end() )
			throw( std::logic_error( "MemoryPool::Data already used." ) );

		it = std::find( _datasUnused.begin(), _datasUnused.end(), data );
		// move from unused to used...
		_datasUsed.transfer( _datasUsed.begin(), // insert before the first element
		                     it, // insert this data ...
		                     _datasUnused ); // from this container
	}

	void setUnused( PoolData& data )
	{
		DataList::iterator it = std::find( _datasUnused.begin(), _datasUnused.end(), data );

		if( it != _datasUnused.end() )
			throw( std::logic_error( "MemoryPool::Data already unused." ) );

		it = std::find( _datasUsed.begin(), _datasUsed.end(), data );
		// move from used to unused...
		_datasUnused.transfer( _datasUnused.begin(), // insert before the first element
		                       it, // insert this data ...
		                       _datasUsed ); // from this container
	}

public:
	//	std::size_t getMemorySizeUsed() const { return _memoryUsed; }
	//	std::size_t getMemorySizeAllocated() const { return _memoryAllocated; }

	std::size_t getMemorySizeUsed() const
	{
		return std::accumulate( _datasUsed.begin(), _datasUsed.end(), 0, std::ptr_fun( &functor_data_size ) );
	}

	std::size_t getMemorySizeAllocated() const
	{
		return getMemorySizeUsed() + std::accumulate( _datasUnused.begin(), _datasUnused.end(), 0, std::ptr_fun( &functor_data_size ) );
	}

	std::size_t getMemorySizeAuthorized() const
	{
		return _memoryAuthorized;
	}

	std::size_t getMemorySizeAvailable() const
	{
		return getMemorySizeAuthorized() - getMemorySizeUsed();
	}

	virtual void clear( std::size_t size );
	virtual void clearOne();
	virtual void clearAll();

private:
	static std::size_t functor_data_size( const std::size_t& sum, const PoolData& x )
	{
		return sum + x.getSize();
	}

	//	static bool data_is_unused( const Data& d )
	//	{
	//		return d._used == false;
	//	}
};

}
}
}

#endif

