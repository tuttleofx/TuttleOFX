#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP

#include <boost/ptr_container/ptr_list.hpp>
#include <vector>
#include <numeric>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace core {

typedef unsigned long long SizeInteger;

class IMemoryPool
{
public:
	virtual SizeInteger getMemorySizeUsed( ) const = 0;
	virtual SizeInteger getMemorySizeAllocated( ) const = 0;
	virtual SizeInteger getMemorySizeAuthorized( ) const = 0;
	virtual SizeInteger clear( SizeInteger size ) = 0;
	virtual SizeInteger clearOne( ) = 0;
	virtual SizeInteger clearAll( ) = 0;
};

class MemoryPool : public IMemoryPool
{
public:

	class Data
	{
		static unsigned long _count;
	public:

		Data( const SizeInteger size ) : _size( size )
		{
			_id = _count++;
			_data = new char[size];
		}

		~Data( )
		{
			delete [] _data;
		}
	public:

		bool operator==(const Data& other )
		{
			return _id == other._id;
		}
	public:
		unsigned long _id; ///< unique id to identify one memory data
		char* _data; ///< own the data
		SizeInteger _size; ///< data size
		//		bool _used; ///< is this data currently used
	};
	typedef boost::ptr_list<Data> DataList;
	DataList _datasUsed;
	DataList _datasUnused;
	SizeInteger _memoryUsed;
	SizeInteger _memoryAllocated;
	SizeInteger _memoryAuthorized;
public:
	MemoryPool( );
	~MemoryPool( );

public:

	Data& allocate( SizeInteger size ) throw(std::bad_alloc, std::length_error )
	{
		if( size > getMemorySizeAuthorized( ) - getMemorySizeAllocated( ) )
		{
			std::stringstream s;
			s << "MemoryPool can't allocate size:" << size << " because memoryAuthorized-memoryAllocated=" << _memoryAuthorized - _memoryAllocated;
			throw std::length_error( s.str( ) );
		}
		Data* data = new Data( size );
		_datasUsed.push_front( data );
		return *data;
	}

	void desallocate( Data& data )
	{
		DataList::iterator it = std::find( _datasUsed.begin( ), _datasUsed.end( ), data );
		if( it != _datasUsed.end() )
		{
			_datasUsed.erase( it );
		}
		else
		{
			it = std::find( _datasUnused.begin( ), _datasUnused.end( ), data );
			if( it != _datasUnused.end() )
			{
				_datasUnused.erase( it );
			}
			else
			{
				throw( std::logic_error("Can't desallocate from the pool, data not found !") );
			}
		}
	}

	void setUsed( Data& data )
	{
		DataList::iterator it = std::find( _datasUsed.begin( ), _datasUsed.end( ), data );
		if( it != _datasUsed.end() )
			throw( std::logic_error( "MemoryPool::Data already used." ) );

		it = std::find( _datasUnused.begin( ), _datasUnused.end( ), data );
		// move from unused to used...
		_datasUsed.transfer( _datasUsed.begin( ), // insert before the first element
							 it, // insert this data ...
							 _datasUnused ); // from this container
	}

	void setUnused( Data& data )
	{
		DataList::iterator it = std::find( _datasUnused.begin( ), _datasUnused.end( ), data );
		if( it != _datasUnused.end() )
			throw( std::logic_error( "MemoryPool::Data already unused." ) );
		
		it = std::find( _datasUsed.begin( ), _datasUsed.end( ), data );
		// move from used to unused...
		_datasUnused.transfer( _datasUnused.begin( ), // insert before the first element
							 it, // insert this data ...
							 _datasUsed ); // from this container
	}


public:
	//	SizeInteger getMemorySizeUsed() const { return _memoryUsed; }
	//	SizeInteger getMemorySizeAllocated() const { return _memoryAllocated; }
	struct data_size : public std::binary_function<SizeInteger, Data, SizeInteger>
	{
	  SizeInteger operator()(const SizeInteger& sum, const Data& x) const
	  {
		return sum + x._size;
	  }
	};
	SizeInteger getMemorySizeUsed( ) const
	{
		return std::accumulate( _datasUsed.begin( ), _datasUsed.end( ), 0, data_size() );
	}

	SizeInteger getMemorySizeAllocated( ) const
	{
		return getMemorySizeUsed( ) + std::accumulate( _datasUnused.begin( ), _datasUnused.end( ), 0, data_size( ) );
	}

	SizeInteger getMemorySizeAuthorized( ) const
	{
		return _memoryAuthorized;
	}
	SizeInteger clear( SizeInteger size );
	SizeInteger clearOne( );
	SizeInteger clearAll( );
private:

//	static bool data_is_unused( const Data& d )
//	{
//		return d._used == false;
//	}
};

}
}
}

#endif

