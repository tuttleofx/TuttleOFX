#include "MemoryCache.hpp"
#include <tuttle/host/attribute/Image.hpp> // to know the function getReference()
#include <tuttle/common/utils/global.hpp>
#include <boost/foreach.hpp>

#include <functional>

namespace tuttle
{
namespace host
{
namespace memory
{

namespace
{

/// Check if the cache element is kept in the cache, but is not required by someone else.
bool isUnused(const CACHE_ELEMENT& cacheElement)
{
    return cacheElement->getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerHost) < 1;
}

/// Functor to get the smallest unused element in cache
struct UnusedDataFitSize : public std::unary_function<CACHE_ELEMENT*, void>
{
    UnusedDataFitSize(std::size_t size)
        : _sizeNeeded(size)
        , _bestMatchDiff(ULONG_MAX)
        , _pBestMatch()
    {
    }

    void operator()(const std::pair<Key, CACHE_ELEMENT>& pData)
    {
        // used data
        if(!isUnused(pData.second))
            return;

        const std::size_t bufferSize = pData.second->getPoolData()->reservedSize();

        // Check minimum amount of memory
        if(_sizeNeeded > bufferSize)
            return;

        const std::size_t diff = bufferSize - _sizeNeeded;
        if(diff >= _bestMatchDiff)
            return;
        _bestMatchDiff = diff;
        _pBestMatch = pData.second;
    }

    CACHE_ELEMENT bestMatch() { return _pBestMatch; }

private:
    const std::size_t _sizeNeeded;
    std::size_t _bestMatchDiff;
    CACHE_ELEMENT _pBestMatch;
};
}

MemoryCache& MemoryCache::operator=(const MemoryCache& cache)
{
    if(&cache == this)
        return *this;
    boost::mutex::scoped_lock lockerMap1(cache._mutexMap);
    boost::mutex::scoped_lock lockerMap2(_mutexMap);
    _map = cache._map;
    return *this;
}

void MemoryCache::put(const std::string& identifier, const double time, CACHE_ELEMENT pData)
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    _map[Key(identifier, time)] = pData;
}

CACHE_ELEMENT MemoryCache::get(const std::string& identifier, const double time) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    MAP::const_iterator itr = _map.find(Key(identifier, time));

    if(itr == _map.end())
        return CACHE_ELEMENT();
    return itr->second;
}

CACHE_ELEMENT MemoryCache::get(const std::size_t& i) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    MAP::const_iterator itr = _map.begin();
    for(unsigned int j = 0; j < i && itr != _map.end(); ++j)
        ++itr;

    if(itr == _map.end())
        return CACHE_ELEMENT();
    return itr->second;
}

CACHE_ELEMENT MemoryCache::getUnusedWithSize(const std::size_t requestedSize) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    return std::for_each(_map.begin(), _map.end(), UnusedDataFitSize(requestedSize)).bestMatch();
}

std::size_t MemoryCache::size() const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    return _map.size();
}

bool MemoryCache::empty() const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    return _map.empty();
}

bool MemoryCache::inCache(const CACHE_ELEMENT& pData) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    return getIteratorForValue(pData) != _map.end();
}

namespace
{

const std::string EMPTY_STRING = "";

template <typename T>
struct FindValuePredicate : public std::unary_function<typename T::value_type, bool>
{
    const typename T::mapped_type& _value;
    FindValuePredicate(const typename T::mapped_type& value)
        : _value(value)
    {
    }

    bool operator()(const typename T::value_type& pair) { return pair.second == _value; }
};
}

MemoryCache::MAP::const_iterator MemoryCache::getIteratorForValue(const CACHE_ELEMENT& pData) const
{
    return std::find_if(_map.begin(), _map.end(), FindValuePredicate<MAP>(pData));
}

MemoryCache::MAP::iterator MemoryCache::getIteratorForValue(const CACHE_ELEMENT& pData)
{
    return std::find_if(_map.begin(), _map.end(), FindValuePredicate<MAP>(pData));
}

double MemoryCache::getTime(const CACHE_ELEMENT& pData) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    MAP::const_iterator itr = getIteratorForValue(pData);

    if(itr == _map.end())
        return 0;
    return itr->first._time;
}

const std::string& MemoryCache::getPluginName(const CACHE_ELEMENT& pData) const
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    MAP::const_iterator itr = getIteratorForValue(pData);

    if(itr == _map.end())
        return EMPTY_STRING;
    return itr->first._identifier;
}

bool MemoryCache::remove(const CACHE_ELEMENT& pData)
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    const MAP::iterator itr = getIteratorForValue(pData);

    if(itr == _map.end())
        return false;
    _map.erase(itr);
    return true;
}

void MemoryCache::clearUnused()
{
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    for(MAP::iterator it = _map.begin(); it != _map.end();)
    {
        if(isUnused(it->second))
        {
            _map.erase(
                it++); // post-increment here, increments 'it' and returns a copy of the original 'it' to be used by erase()
        }
        else
        {
            ++it;
        }
    }
}

void MemoryCache::clearAll()
{
    TUTTLE_LOG_DEBUG(" - MEMORYCACHE::CLEARALL - ");
    boost::mutex::scoped_lock lockerMap(_mutexMap);
    _map.clear();
}

std::ostream& operator<<(std::ostream& os, const MemoryCache& v)
{
    os << "[MemoryCache] size:" << v.size() << std::endl;
    BOOST_FOREACH(const MemoryCache::MAP::value_type& i, v._map)
    {
        os << "[MemoryCache] " << i.first << " id:" << i.second->getId()
           << " ref host:" << i.second->getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerHost)
           << " ref plugins:" << i.second->getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerPlugin)
           << std::endl;
    }
    return os;
}
}
}
}
