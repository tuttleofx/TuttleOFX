#ifndef _TUTTLE_HOST_CORE_MEMORYPOOL_HPP
#define _TUTTLE_HOST_CORE_MEMORYPOOL_HPP

#include <list>
#include <vector>
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
	virtual SizeInteger getMemorySizeUsed() const =0;
	virtual SizeInteger getMemorySizeAllocated() const =0;
	virtual SizeInteger getMemorySizeAuthorized() const =0;
	virtual SizeInteger clear(SizeInteger size)=0;
	virtual SizeInteger clearOne()=0;
	virtual SizeInteger clearAll()=0;
};

class MemoryPool : public IMemoryPool
{
public:
	class Data
	{
		static unsigned long _count;
	public:
		unsigned long _id;
	public:
		Data( char* data, const SizeInteger size ):_data(data),_size(size){ _id = _count++; }
		virtual ~Data(){}
		bool operator==(const Data& other){ return _id == other._id; }
		char* _data; ///< only a link to the data
		SizeInteger _size; ///< the size of the datas
		bool _used; ///< is this data currently used
	};
	std::list<Data> _datas;
	std::vector<IMemoryPool> _subPools;
	SizeInteger _memoryUsed;
	SizeInteger _memoryAllocated;
	SizeInteger _memoryAuthorized;
public:
	MemoryPool();
	~MemoryPool();

public:
	Data& allocate( SizeInteger size ) throw(std::bad_alloc, std::length_error)
	{
		if( size < _memoryAuthorized - _memoryAllocated )
		{
			std::stringstream s;
			s << "MemoryPool can't allocate size:"<<size<<" because memoryAuthorized-memoryAllocated="<<_memoryAuthorized - _memoryAllocated;
			throw std::length_error(s.str());
		}
		char* data = new char[size];
		_datas.push_back( Data( data, size ) );
	}
	void desallocate( Data& data )
	{
		delete[] data._data;
		_datas.remove( data );
	}


public:
	SizeInteger getMemorySizeUsed() const { return _memoryUsed; }
	SizeInteger getMemorySizeAllocated() const { return _memoryAllocated; }
	SizeInteger getMemorySizeAuthorized() const { return _memoryAuthorized; }
	SizeInteger clear(SizeInteger size);
	SizeInteger clearOne();
	SizeInteger clearAll();
private:
	static bool data_is_unused( const Data& d ){ return d._used == false; }
};

}
}
}

#endif

