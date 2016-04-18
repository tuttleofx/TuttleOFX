#define BOOST_TEST_MODULE test_function
#include <tuttle/test/main.hpp>

#include <iostream>

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(test_01)
{
    BOOST_CHECK_EQUAL(1.0, 1.0);
}
