#include "MemoryPool.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/system/memoryInfo.hpp>
#include <tuttle/host/Core.hpp>

#include <boost/throw_exception.hpp>

#include <algorithm>

namespace tuttle
{
namespace host
{
namespace memory
{

IPool::~IPool()
{
}

class PoolData : public IPoolData
{
private:
    PoolData();                ///< No default Ctor
    PoolData(const PoolData&); ///< No copy Ctor
    friend class MemoryPool;

public:
    PoolData(IPool& pool, const std::size_t size)
        : _pool(pool)
        , _id(_count++)
        , _reservedSize(size)
        , _size(size)
        , _pData(new char[size])
        , _refCount(0)
    {
    }

    ~PoolData() { delete[] _pData; }

public:
    bool operator==(const PoolData& other) const { return _id == other._id; }

    void addRef();
    void release();

    char* data() { return _pData; }
    const char* data() const { return _pData; }
    const std::size_t size() const { return _size; }
    const std::size_t reservedSize() const { return _reservedSize; }

    void setSize(const std::size_t newSize)
    {
        assert(newSize <= _reservedSize);
        _size = newSize;
    }

private:
    static std::size_t _count;       ///< unique id generator
    IPool& _pool;                    ///< ref to the owner pool
    const std::size_t _id;           ///< unique id to identify one memory data
    const std::size_t _reservedSize; ///< memory allocated
    std::size_t _size;               ///< memory requested
    char* const _pData;              ///< own the data
    int _refCount;                   ///< counter on clients currently using this data
};

void intrusive_ptr_add_ref(IPoolData* pData)
{
    pData->addRef();
}

void intrusive_ptr_release(IPoolData* pData)
{
    pData->release();
}

std::size_t PoolData::_count = 0;

void PoolData::addRef()
{
    if(++_refCount == 1)
        _pool.referenced(this);
}

void PoolData::release()
{
    if(--_refCount == 0)
        _pool.released(this);
}

MemoryPool::MemoryPool(const std::size_t maxSize)
    : _memoryAuthorized(maxSize)
{
}

MemoryPool::~MemoryPool()
{
    if(!_dataUsed.empty())
    {
        TUTTLE_LOG_DEBUG(
            "[Memory Pool] Error inside memory pool. Some data always mark used at the destruction (nb elements:"
            << _dataUsed.size() << ")");
    }
}

void MemoryPool::referenced(PoolData* pData)
{
    boost::mutex::scoped_lock locker(_mutex);
    DataList::iterator it = _dataUnused.find(pData);

    if(it != _dataUnused.end())
    {
        _dataUnused.erase(it);
    }
    else // a really new data
    {
        _allDatas.push_back(pData);
        _dataMap[pData->data()] = pData;
    }
    _dataUsed.insert(pData);
}

void MemoryPool::released(PoolData* pData)
{
    boost::mutex::scoped_lock locker(_mutex);
    _dataUsed.erase(pData);
    _dataUnused.insert(pData);
}

namespace
{

/// Functor to get the smallest element in pool
struct DataFitSize : public std::unary_function<PoolData*, void>
{
    DataFitSize(std::size_t size)
        : _sizeNeeded(size)
        , _bestMatchDiff(ULONG_MAX)
        , _pBestMatch(NULL)
    {
    }

    void operator()(PoolData* pData)
    {
        const std::size_t dataSize = pData->reservedSize();

        // Check minimum amount of memory
        if(_sizeNeeded > dataSize)
            return;
        // Do not reuse too big buffers
        if(dataSize > _maxBufferRatio * _sizeNeeded)
            return;
        const std::size_t diff = dataSize - _sizeNeeded;
        if(diff >= _bestMatchDiff)
            return;
        _bestMatchDiff = diff;
        _pBestMatch = pData;
    }

    PoolData* bestMatch() { return _pBestMatch; }

private:
    static const double _maxBufferRatio; //< max ratio between used and unused part of the buffer
    const std::size_t _sizeNeeded;
    std::size_t _bestMatchDiff;
    PoolData* _pBestMatch;
};

const double DataFitSize::_maxBufferRatio = 2.0;
}

IPoolDataPtr MemoryPool::allocate(const std::size_t size)
{
    // Try to reuse a buffer available in the MemoryPool
    IPoolData* pData = getOneAvailableData(size);
    if(pData != NULL)
    {
        TUTTLE_LOG_TRACE("[Memory Pool] Reuse a buffer available in the MemoryPool");
        pData->setSize(size);
        return pData;
    }

    // Try to remove unused element in MemoryCache, and reuse the buffer available in the MemoryPool
    memory::IMemoryCache& memoryCache = core().getMemoryCache();
    CACHE_ELEMENT unusedCacheElement = memoryCache.getUnusedWithSize(size);
    if(unusedCacheElement.get() != NULL)
    {
        TUTTLE_LOG_TRACE("[Memory Pool] Pop element in the MemoryCache from " << unusedCacheElement->getFullName()
                                                                              << " of size " << size);
        memoryCache.remove(unusedCacheElement);

        pData = getOneAvailableData(size);
        if(pData != NULL)
        {
            TUTTLE_LOG_TRACE("[Memory Pool] Reuse a buffer available in the MemoryPool");
            pData->setSize(size);
            return pData;
        }
    }

    // Try to allocate a new buffer in MemoryPool
    std::size_t availableSize = getAvailableMemorySize();
    if(size > availableSize)
    {
        // Try to release elements from the MemoryCache (make them available to the MemoryPool)
        TUTTLE_LOG_TRACE("[Memory Pool] Release elements from the MemoryCache");
        memoryCache.clearUnused();

        availableSize = getAvailableMemorySize();
        if(size > availableSize)
        {
            // Release elements from the MemoryPool (make them available to the OS)
            TUTTLE_LOG_TRACE("[Memory Pool] Release elements from the MemoryPool");
            clear();
        }

        availableSize = getAvailableMemorySize();
        if(size > availableSize)
        {
            std::stringstream s;
            s << "[Memory Pool] can't allocate size:" << size << " because memory available is equal to " << availableSize
              << " bytes";
            BOOST_THROW_EXCEPTION(std::length_error(s.str()));
        }
    }

    // Allocate a new buffer in MemoryPool
    TUTTLE_LOG_TRACE("[Memory Pool] allocate " << size << " bytes");
    return new PoolData(*this, size);
}

std::size_t MemoryPool::updateMemoryAuthorizedWithRAM()
{
    _memoryAuthorized = /*getUsedMemorySize() +*/ getMemoryInfo()._totalRam;
    TUTTLE_LOG_DEBUG("[Memory Pool] update memory authorized with RAM: " << _memoryAuthorized);
    return _memoryAuthorized;
}

namespace
{

std::size_t accumulateReservedSize(const std::size_t& sum, const IPoolData* pData)
{
    return sum + pData->reservedSize();
}

std::size_t accumulateWastedSize(const std::size_t& sum, const IPoolData* pData)
{
    return sum + (pData->reservedSize() - pData->size());
}
}

std::size_t MemoryPool::getUsedMemorySize() const
{
    boost::mutex::scoped_lock locker(_mutex);
    return std::accumulate(_dataUsed.begin(), _dataUsed.end(), 0, &accumulateReservedSize);
}

std::size_t MemoryPool::getAllocatedAndUnusedMemorySize() const
{
    boost::mutex::scoped_lock locker(_mutex);
    return std::accumulate(_dataUnused.begin(), _dataUnused.end(), 0, &accumulateReservedSize);
}

std::size_t MemoryPool::getAllocatedMemorySize() const
{
    return getUsedMemorySize() + getAllocatedAndUnusedMemorySize();
}

std::size_t MemoryPool::getMaxMemorySize() const
{
    return _memoryAuthorized;
}

std::size_t MemoryPool::getAvailableMemorySize() const
{
    return getMaxMemorySize() - getUsedMemorySize();
}

std::size_t MemoryPool::getWastedMemorySize() const
{
    boost::mutex::scoped_lock locker(_mutex);
    return std::accumulate(_dataUsed.begin(), _dataUsed.end(), 0, std::ptr_fun(&accumulateWastedSize));
}

std::size_t MemoryPool::getDataUsedSize() const
{
    return _dataUsed.size();
}

std::size_t MemoryPool::getDataUnusedSize() const
{
    return _dataUnused.size();
}

PoolData* MemoryPool::getOneAvailableData(const size_t size)
{
    boost::mutex::scoped_lock locker(_mutex);
    return std::for_each(_dataUnused.begin(), _dataUnused.end(), DataFitSize(size)).bestMatch();
}

void MemoryPool::clear(std::size_t size)
{
    /// @todo tuttle
}

void MemoryPool::clear()
{
    boost::mutex::scoped_lock locker(_mutex);
    _dataUnused.clear();
}

void MemoryPool::clearOne()
{
    boost::mutex::scoped_lock locker(_mutex);
    _dataUnused.erase(_dataUnused.begin());
}

std::ostream& operator<<(std::ostream& os, const MemoryPool& memoryPool)
{
    os << "[Memory Pool] Unused data:           " << memoryPool.getDataUnusedSize() << " bytes\n";
    os << "[Memory Pool] All datas:             " << memoryPool.getDataUsedSize() << " bytes\n";
    os << "[Memory Pool] Total RAM:             " << getMemoryInfo()._totalRam << " bytes\n";
    os << "\n";
    os << "[Memory Pool] Used memory:           " << memoryPool.getUsedMemorySize() << " bytes\n";
    os << "[Memory Pool] Allocated memory:      " << memoryPool.getAllocatedMemorySize() << " bytes\n";
    os << "[Memory Pool] Max memory:            " << memoryPool.getMaxMemorySize() << " bytes\n";
    os << "[Memory Pool] Available memory size: " << memoryPool.getAvailableMemorySize() << " bytes\n";
    os << "[Memory Pool] Wasted memory:         " << memoryPool.getWastedMemorySize() << " bytes\n";
    return os;
}
}
}
}
