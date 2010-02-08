#include <iostream>
#define BOOST_TEST_MODULE test_function
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( test_01 )
{
	BOOST_CHECK_EQUAL( 1.0, 1.0 );
}
