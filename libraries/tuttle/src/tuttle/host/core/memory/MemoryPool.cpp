#include "MemoryPool.hpp"
#include <algorithm>

namespace tuttle {
namespace host {
namespace core {

void intrusive_ptr_add_ref(IPoolData *pData){
	pData->addRef();
}

void intrusive_ptr_release(IPoolData *pData){
	pData->release();
}

size_t PoolData::_count = 0;

void PoolData::addRef() {
	if(++_refCount==1)
		_pool.referenced(this);
}

void PoolData::release() {
	if(--_refCount == 0)
		_pool.released(this);
}

MemoryPool::MemoryPool( const size_t maxSize )
	: _memoryAuthorized( maxSize )
{}

MemoryPool::~MemoryPool()
{}


void MemoryPool::referenced(PoolData *pData){
	_dataUnused.remove( pData );
	_dataUsed.push_front( pData );
}

void MemoryPool::released(PoolData *pData){
	_dataUsed.remove( pData );
	_dataUnused.push_front( pData );
}

namespace  {

struct DataFitSize : public std::unary_function<PoolData*, void>{
	DataFitSize(size_t size) :
		_size(size),
		_bestMatchDiff( ULONG_MAX ),
		_pBestMatch(NULL)
		{}

	void operator()(PoolData *pData) {
		const size_t dataSize = pData->reservedSize();
		if( _size > dataSize )
			return;
		const size_t diff = dataSize - _size;
		if( diff >= _bestMatchDiff )
			return;
		_bestMatchDiff = diff;
		_pBestMatch = pData;
	}

	PoolData* bestMatch(){
		return _pBestMatch;
	}
private:
	const size_t _size;
	size_t _bestMatchDiff;
	PoolData *_pBestMatch;
};

}  // namespace

boost::intrusive_ptr<IPoolData> MemoryPool::allocate( const size_t size ) throw( std::bad_alloc, std::length_error )
{
	// checking within unused data
	PoolData * const pData = std::for_each( _dataUnused.begin(), _dataUnused.end(), DataFitSize(size) ).bestMatch();
	if( pData != NULL ){
		pData->_size = size;
		return pData;
	}
	if( size > getAvailableMemorySize() )
	{
		std::stringstream s;
		s << "MemoryPool can't allocate size:" << size << " because memorySizeAvailable=" << getAvailableMemorySize();
		throw std::length_error( s.str() );
	}
	return new PoolData( *this, size );
}

namespace  {

static size_t accumulateReservedSize( const size_t& sum, const IPoolData* pData )
{
	return sum + pData->reservedSize();
}

static size_t accumulateWastedSize( const size_t& sum, const IPoolData* pData )
{
	return sum + ( pData->reservedSize() - pData->size() );
}

}  // namespace

size_t MemoryPool::getUsedMemorySize() const
{
	return std::accumulate( _dataUsed.begin(), _dataUsed.end(), 0, &accumulateReservedSize );
}

size_t MemoryPool::getAllocatedMemorySize() const
{
	return getUsedMemorySize() + std::accumulate( _dataUnused.begin(), _dataUnused.end(), 0, &accumulateReservedSize );
}

size_t MemoryPool::getMaxMemorySize() const
{
	return _memoryAuthorized;
}

size_t MemoryPool::getAvailableMemorySize() const
{
	return getMaxMemorySize() - getUsedMemorySize();
}

size_t MemoryPool::getWastedMemorySize() const
{
	return std::accumulate( _dataUsed.begin(), _dataUsed.end(), 0, std::ptr_fun( &accumulateWastedSize ) );
}

void MemoryPool::clear( size_t size )
{}

void MemoryPool::clearOne()
{}

void MemoryPool::clearAll()
{
	_dataUnused.clear();
}

}
}
}
