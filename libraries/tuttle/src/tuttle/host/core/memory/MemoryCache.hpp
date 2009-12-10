#ifndef _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_MEMORYCACHE_HPP_

#include "IMemoryCache.hpp"
#include "MemoryPool.hpp"

#include "boost/foreach.hpp"
#include <boost/ptr_container/ptr_map.hpp>
#include <list>
#include <cstring>

namespace tuttle {
namespace host {
namespace core {

class MemoryCache // : public IMemoryCache
{
public:
	class CacheData
	{
	public:
		//typedef std::list<MemoryPool::Data*> PoolDataList;

	public:
		CacheData( MemoryPool::PoolData& data );
		~CacheData() {}

	public:
		std::size_t           getSize() const { return _data.getSize(); }
		MemoryPool::PoolData& getData()       { return _data; }
		bool                  isUsed()        { return _isUsed; }
		int                   getWillBeUsed() { return _willBeUsed; }
		int                   getWeight()     { return _weight; }

	private:
		//PoolDataList _datas;
		MemoryPool::PoolData& _data;
		bool _isUsed; ///< this data is used
		int _willBeUsed; ///< we known that this data will be used x times in the future
		int _weight; ///< a coefficent representing the priority to keep this data (maybe the complexity to recalculate from scratch)
	};
	typedef std::pair<std::string, double> Key;
	typedef boost::ptr_map<Key, CacheData> DataMap;
	typedef boost::ptr_container_detail::ref_pair<Key, const CacheData* const> DataMapContent;
	typedef std::list<Key> KeyList;
	DataMap _datas;
	KeyList _keys; ///< sort by last usage

public:
	MemoryCache( MemoryPool& pool );
	~MemoryCache();

private:
	MemoryPool& _pool;

public:
	MemoryPool::PoolData& get( const std::string& id, const double t )
	{
		return _datas.at( Key( id, t ) ).getData();
	}

	bool add( const std::string& id, const double t, MemoryPool::PoolData& data )
	{
		Key k( id, t );

		_datas.insert( k, new CacheData( data ) );
		_keys.push_front( k );
	}

	bool release( const std::pair<std::string, double>& key )
	{
		release( key.first, key.second );
	}

	bool release( std::string id, double t )
	{
		Key k( id, t );

		_pool.setUnused( _datas.at( k ).getData() );
		_datas.erase( k );
		_keys.remove( k );
	}

	bool purge()
	{
		for( DataMap::iterator it = _datas.begin(), itEnd = _datas.end();
		     it != itEnd;
		     ++it )
		{
			CacheData* d = it->second;
			if( !d->isUsed() )
			{
				release( it->first );
			}
		}
	}

	std::size_t getMemorySizeUsed() const
	{
		std::size_t counter = 0;
		BOOST_FOREACH( DataMapContent p, _datas )
		{
			counter += p.second->getSize();
		}
		return counter;
	}

};

}
}
}

#endif

