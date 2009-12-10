// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/core/memory/MemoryPool.hpp>
#include <tuttle/host/core/memory/MemoryCache.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( memory_tests_suite01 )

BOOST_AUTO_TEST_CASE( memoryPool )
{
	using namespace std;
	using namespace tuttle::host;

	core::MemoryPool pool;

	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );

	std::size_t size        = 123;
	core::MemoryPool::PoolData& d = pool.allocate( size );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
	pool.setUnused( d );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
	pool.setUsed( d );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
	pool.setUnused( d );
	pool.clearAll();
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );

	std::size_t size1        = 222;
	std::size_t size2        = 333;
	core::MemoryPool::PoolData& d1 = pool.allocate( size1 );
	core::MemoryPool::PoolData& d2 = pool.allocate( size2 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size1 + size2 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size1 + size2 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAvailable(), pool.getMemorySizeAuthorized() - pool.getMemorySizeUsed() );
}

BOOST_AUTO_TEST_CASE( memoryCache )
{
	using namespace std;
	using namespace tuttle::host;

	core::MemoryPool pool;
	core::MemoryCache cache( pool );

	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );

	std::size_t size        = 123;
	core::MemoryPool::PoolData& d = pool.allocate( size );
	cache.add( "d", 12, d );
	char* buffer = cache.get( "d", 12 ).getData();
	BOOST_CHECK_EQUAL( buffer, d.getData() );
	BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );

	cache.release( "d", 12 );
	/*
	 * pool.setUnused( d );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
	 * pool.setUsed( d );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size );
	 * pool.setUnused( d );
	 * pool.clearAll();
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), 0 );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), 0 );
	 *
	 * std::size_t size1 = 222;
	 * std::size_t size2 = 333;
	 * core::MemoryPool::Data& d1 = pool.allocate( size1 );
	 * core::MemoryPool::Data& d2 = pool.allocate( size2 );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeAllocated(), size1+size2 );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeUsed(), size1+size2 );
	 * BOOST_CHECK_EQUAL( pool.getMemorySizeAvailable(), pool.getMemorySizeAuthorized()-pool.getMemorySizeUsed() );
	 */
}

BOOST_AUTO_TEST_SUITE_END()

