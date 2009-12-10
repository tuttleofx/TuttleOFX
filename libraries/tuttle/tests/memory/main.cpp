// custom host
#include <tuttle/host/core/memory/MemoryPool.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( memory_tests_suite01 )

BOOST_AUTO_TEST_CASE( memoryPool )
{
	using namespace std;
	using namespace tuttle::host::core;

	BOOST_REQUIRE_THROW( {
		MemoryPool pool(0);
		BOOST_CHECK_EQUAL(0U, pool.getMaxMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getAllocatedMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );
		pool.allocate(10);
	}, std::exception );

	MemoryPool pool(30);
	// checking everything is clean
	BOOST_CHECK_EQUAL(30U, pool.getMaxMemorySize() );
	BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize() );
	BOOST_CHECK_EQUAL(0U, pool.getAllocatedMemorySize() );
	BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );

	{ // first allocation
		const IPoolDataPtr pData = pool.allocate(10);
		BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );
	}
	// memory is still allocated but no more used
	BOOST_CHECK_EQUAL(0U, pool.getUsedMemorySize() );
	BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize() );
	BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );

	{ // new allocation will not actually alloc but resuse memory
		const IPoolDataPtr pData = pool.allocate(10);
		BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );
	}
	{ // same thing for a smaller allocation
		const IPoolDataPtr pData = pool.allocate(1);
		BOOST_CHECK_EQUAL(1U, pData->size() );
		BOOST_CHECK_EQUAL(10U, pData->reservedSize() );
		BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(10U, pool.getAllocatedMemorySize() );
		BOOST_CHECK_EQUAL(9U, pool.getWastedMemorySize() );
	}
	{ // asking for a bigger alloc will alloc this time
		const IPoolDataPtr pData = pool.allocate(15);
		BOOST_CHECK_EQUAL(15U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(25U, pool.getAllocatedMemorySize() );
		BOOST_CHECK_EQUAL(0U, pool.getWastedMemorySize() );
	}
	{
		// asking for a new block that could be satisfied by the
		// two previously allocated block will return the smallest
		const IPoolDataPtr pData = pool.allocate(9);
		BOOST_CHECK_EQUAL(10U, pData->reservedSize() );
		BOOST_CHECK_EQUAL(9U, pData->size() );
		BOOST_CHECK_EQUAL(1U, pool.getWastedMemorySize() );

		// no new allocation should have take place
		BOOST_CHECK_EQUAL(10U, pool.getUsedMemorySize() );
		BOOST_CHECK_EQUAL(25U, pool.getAllocatedMemorySize() );

		const char* const_data = pData->data();
		char* data = pData->data();
		data[0] = const_data[0];
	}
	BOOST_REQUIRE_THROW( pool.allocate(50), std::exception );

//	core::MemoryPool pool;
//
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
//
//	size_t size        = 10;
//	core::MemoryPool::PoolData& d = pool.allocate( size );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
//	pool.setUnused( d );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
//	pool.setUsed( d );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
//	pool.setUnused( d );
//	pool.clearAll();
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
//
//	size_t size1 = 222;
//	size_t size2 = 333;
//	core::MemoryPool::PoolData& d1 = pool.allocate( size1 );
//	core::MemoryPool::PoolData& d2 = pool.allocate( size2 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size1 + size2 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size1 + size2 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAvailable(), pool.getMemorySizeAuthorized() - pool.getMemorySizeUsed() );
//	pool.deallocate(d2);
//	pool.deallocate(d1);
}

//BOOST_AUTO_TEST_CASE( memoryCache )
//{
//	using namespace std;
//	using namespace tuttle::host;
//
//	core::MemoryPool pool;
//	core::MemoryCache cache( pool );
//
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
//
//	std::size_t size        = 123;
//	core::MemoryPool::PoolData& d = pool.allocate( size );
//	cache.add( "d", 12, d );
//	char* buffer = cache.get( "d", 12 );
//	BOOST_CHECK_EQUAL( buffer, d );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
//	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
//
//	cache.release( "d", 12 );
//}

BOOST_AUTO_TEST_SUITE_END()

