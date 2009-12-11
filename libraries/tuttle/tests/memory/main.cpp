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
}

BOOST_AUTO_TEST_SUITE_END()

