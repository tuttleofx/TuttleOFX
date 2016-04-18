#define BOOST_TEST_MODULE tuttle_memory
#include <tuttle/test/main.hpp>

// custom host
#include <tuttle/host/memory/MemoryPool.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <iostream>

using namespace boost::unit_test;
using namespace std;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(memory_tests_suite01)

BOOST_AUTO_TEST_CASE(memoryPool)
{
    {
        // empty pool
        memory::MemoryPool emptyPool;
        BOOST_CHECK_EQUAL(0U, emptyPool.getMaxMemorySize());
        BOOST_CHECK_EQUAL(0U, emptyPool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(0U, emptyPool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(0U, emptyPool.getWastedMemorySize());
        BOOST_REQUIRE_THROW(emptyPool.allocate(10), std::exception);
    }

    memory::MemoryPool pool(30);
    // checking everything is clean
    BOOST_CHECK_EQUAL(30U, pool.getMaxMemorySize());
    BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize());
    BOOST_CHECK_EQUAL(0U, pool.getAllocatedMemorySize());
    BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());

    // can't allocate more than size of pool
    BOOST_REQUIRE_THROW(pool.allocate(50), std::exception);

    {
        // first allocation
        const memory::IPoolDataPtr pData = pool.allocate(10);
        BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());
    }
    // memory is still allocated but no more used
    BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize());
    BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize());
    BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());

    {
        // new allocation will not actually alloc but resuse memory
        const memory::IPoolDataPtr pData = pool.allocate(10);
        BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());
    }
    {
        // same thing for a smaller allocation (but bigger than half of the allocated buffer)
        // resuse memory
        const memory::IPoolDataPtr pData = pool.allocate(6);
        BOOST_CHECK_EQUAL(6U, pData->size());
        BOOST_CHECK_EQUAL(10U, pData->reservedSize());
        BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(4U, pool.getWastedMemorySize());
    }
    {
        // same thing for a smaller allocation (smaller than half of the allocated buffer)
        // allocate new buffer
        const memory::IPoolDataPtr pData = pool.allocate(4);
        BOOST_CHECK_EQUAL(4U, pData->size());
        BOOST_CHECK_EQUAL(4U, pData->reservedSize());
        BOOST_CHECK_EQUAL(4U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(14U, pool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());
    }
    {
        // asking for a bigger alloc will alloc this time
        const memory::IPoolDataPtr pData = pool.allocate(15);
        BOOST_CHECK_EQUAL(15U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(29U, pool.getAllocatedMemorySize());
        BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize());
    }
    {
        // asking for a new block that could be satisfied by the
        // two previously allocated block will return the smallest
        const memory::IPoolDataPtr pData = pool.allocate(9);
        BOOST_CHECK_EQUAL(10U, pData->reservedSize());
        BOOST_CHECK_EQUAL(9U, pData->size());
        BOOST_CHECK_EQUAL(1U, pool.getWastedMemorySize());

        // no new allocation should have take place
        BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(29U, pool.getAllocatedMemorySize());

        const char* const_data = pData->data();
        char* data = pData->data();
        data[0] = const_data[0];
    }
    {
        // clear memory allocated
        BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(29U, pool.getAllocatedMemorySize());
        pool.clear();
        BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize());
        BOOST_CHECK_EQUAL(0U, pool.getAllocatedMemorySize());
    }
}

BOOST_AUTO_TEST_CASE(memoryCache)
{
    memory::MemoryCache cache;

    // checking initial state
    BOOST_CHECK_EQUAL(true, cache.empty());
    BOOST_CHECK_EQUAL(0U, cache.size());

    const string plugName("name");
    const double time = 10;
    memory::CACHE_ELEMENT pData;

    // testing put function
    cache.put(plugName, time, pData);
    BOOST_CHECK_EQUAL(false, cache.empty());
    BOOST_CHECK_EQUAL(1U, cache.size());
    BOOST_CHECK_EQUAL(true, cache.inCache(pData));
    BOOST_CHECK_EQUAL(plugName, cache.getPluginName(pData));
    BOOST_CHECK_EQUAL(time, cache.getTime(pData));

    // testing clearAll function
    cache.clearAll();
    BOOST_CHECK_EQUAL(true, cache.empty());
    BOOST_CHECK_EQUAL(0U, cache.size());
    BOOST_CHECK_EQUAL(false, cache.inCache(pData));

    // putting data in cache for a second time
    cache.put(plugName, time, pData);
    BOOST_CHECK_EQUAL(true, cache.inCache(pData));

    // testing remove function
    cache.remove(pData);
    BOOST_CHECK_EQUAL(true, cache.empty());
    BOOST_CHECK_EQUAL(0U, cache.size());
    BOOST_CHECK_EQUAL(false, cache.inCache(pData));

    // putting twice the same element
    cache.put(plugName, time, pData);
    cache.put(plugName, time, pData);
    BOOST_CHECK_EQUAL(false, cache.empty());
    BOOST_CHECK_EQUAL(1U, cache.size());
    BOOST_CHECK_EQUAL(true, cache.inCache(pData));
}

BOOST_AUTO_TEST_SUITE_END()
