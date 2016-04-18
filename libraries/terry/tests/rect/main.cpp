#include <terry/globals.hpp>
#include <terry/math/Rect.hpp>

#include <iostream>

#define BOOST_TEST_MODULE terry_math_rect_tests
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(terry_math_rect_tests_suite01)

BOOST_AUTO_TEST_CASE(access)
{
    terry::Rect<double> r(11, 22, 33, 44);

    BOOST_CHECK_EQUAL(r[0], r.x1);
    BOOST_CHECK_EQUAL(r[1], r.y1);
    BOOST_CHECK_EQUAL(r[2], r.x2);
    BOOST_CHECK_EQUAL(r[3], r.y2);
}

BOOST_AUTO_TEST_SUITE_END()
