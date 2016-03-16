#include <iostream>

#define BOOST_TEST_MODULE test_plugin_lut
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(plugin_lut)

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(plugin_lut_test_01)
{
    BOOST_CHECK_EQUAL(1.0, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
