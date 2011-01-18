#include "MemoryCache.hpp"
#include <tuttle/host/attribute/Image.hpp> // to know the function getReference()
#include <tuttle/common/utils/global.hpp>
#include <boost/foreach.hpp>

#include <functional>

namespace tuttle {
namespace host {
namespace memory {

MemoryCache& MemoryCache::operator=( const MemoryCache& cache )
{
	if( &cache == this )
		return *this;
	boost::mutex::scoped_lock lockerMap1( cache._mutexMap );
	boost::mutex::scoped_lock lockerMap2( _mutexMap );
	_map = cache._map;
	return *this;
}

void MemoryCache::put( const std::string& identifier, const double time, CACHE_ELEMENT pData )
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	_map[Key( identifier, time )] = pData;
}

CACHE_ELEMENT MemoryCache::get( const std::string& identifier, const double time ) const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	MAP::const_iterator itr = _map.find( Key( identifier, time ) );

	if( itr == _map.end() )
		return CACHE_ELEMENT();
	return itr->second;
}

std::size_t MemoryCache::size() const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	return _map.size();
}

bool MemoryCache::empty() const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	return _map.empty();
}

bool MemoryCache::inCache( const CACHE_ELEMENT& pData ) const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	return getIteratorForValue( pData ) != _map.end();
}

namespace {

const std::string EMPTY_STRING = "";

template<typename T>
struct FindValuePredicate : public std::unary_function<typename T::value_type, bool>
{
	const typename T::mapped_type & _value;
	FindValuePredicate( const typename T::mapped_type& value ) : _value( value ) {}

	bool operator()( const typename T::value_type& pair )
	{
		return pair.second == _value;
	}

};

}

MemoryCache::MAP::const_iterator MemoryCache::getIteratorForValue( const CACHE_ELEMENT& pData ) const
{
	return std::find_if( _map.begin(), _map.end(), FindValuePredicate<MAP>( pData ) );
}

MemoryCache::MAP::iterator MemoryCache::getIteratorForValue( const CACHE_ELEMENT& pData )
{
	return std::find_if( _map.begin(), _map.end(), FindValuePredicate<MAP>( pData ) );
}

double MemoryCache::getTime( const CACHE_ELEMENT& pData ) const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	MAP::const_iterator itr = getIteratorForValue( pData );

	if( itr == _map.end() )
		return 0;
	return itr->first._time;
}

const std::string& MemoryCache::getPluginName( const CACHE_ELEMENT& pData ) const
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	MAP::const_iterator itr = getIteratorForValue( pData );

	if( itr == _map.end() )
		return EMPTY_STRING;
	return itr->first._identifier;
}

bool MemoryCache::remove( const CACHE_ELEMENT& pData )
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	const MAP::iterator itr = getIteratorForValue( pData );

	if( itr == _map.end() )
		return false;
	_map.erase( itr );
	return true;
}

void MemoryCache::clearUnused()
{
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	for( MAP::iterator it = _map.begin(); it != _map.end(); )
	{
		if( it->second->getReference() <= 1 )
		{
			_map.erase( it++ ); // post-increment here, increments 'it' and returns a copy of the original 'it' to be used by erase()
		}
		else
		{
			++it;
		}
	}
}

void MemoryCache::clearAll()
{
	TUTTLE_TCOUT( " - MEMORYCACHE::CLEARALL - " );
	boost::mutex::scoped_lock lockerMap( _mutexMap );
	_map.clear();
}

std::ostream& operator<<( std::ostream& os, const MemoryCache& v )
{
	os << "size:" << v.size() << std::endl;
	BOOST_FOREACH( const MemoryCache::MAP::value_type& i, v._map )
	{
		os << i.first << " id:" << i.second->getId() << " ref:" << i.second->getReference() << std::endl;
	}
	return os;
}

}
}
}
