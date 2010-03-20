#include <iostream>
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE "test function"

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( test_01 )
{

	BOOST_CHECK_EQUAL( 1.0, 1.0 );
}
