#include "MemoryCache.hpp"
#include <boost/foreach.hpp>

namespace tuttle {
namespace host {
namespace core {

bool MemoryCache::Key::operator<(const Key&other) const {
	if(time!=other.time)
		return time < other.time;
	return pluginName < other.pluginName;
}

MemoryCache::MemoryCache(IMemoryPool& pool) : _pool(pool)
{
}

MemoryCache::~MemoryCache() {
}

void MemoryCache::put(const std::string& pluginName, const double &time,IPoolDataPtr pData)
{
	_map[Key(pluginName, time)] = pData;
}

IPoolDataPtr& MemoryCache::get(const std::string& pluginName, const double &time) const
{
}

std::size_t MemoryCache::size() const
{
	return _map.size();
}

bool MemoryCache::empty() const
{
	return _map.empty();
}

bool MemoryCache::inCache(const IPoolDataPtr &pData) const
{
	BOOST_FOREACH(const MAP::value_type& pair, _map){
		if(pair.second == pData)
			return true;
	}
	return false;
}

namespace {

#include <functional>
template<typename T>
struct FindValuePredicate : public std::unary_function<T::value_type,bool>
{
	const T::mapped_type& _value;
	FindValuePredicate(const T::mapped_type& value) : _value(value){}

	bool operator()(const T::value_type& pair)
	{
		return pair.second == _value;
	}
}
}  // namespace


double MemoryCache::getTime(const IPoolDataPtr &pData) const
{
	MAP::const_iterator itr = _map.find_if(_map.begin(), _map.end(), FindValuePredicate(pData));

}

const std::string& MemoryCache::getPluginName(const IPoolDataPtr &) const
{
}

bool MemoryCache::remove(const IPoolDataPtr &)
{
}

void MemoryCache::clearAll()
{
}

}
}
}
