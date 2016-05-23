#include <terry/globals.hpp>
#include <terry/filter/sobel.hpp>

#include <iostream>

#define BOOST_TEST_MODULE terry_filter_tests
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(terry_filter_sobel_tests_suite01)

BOOST_AUTO_TEST_CASE(sobel)
{
    // empty input/output, the test only check the compilation
    terry::rgb32f_view_t inView;
    terry::gray32f_view_t outViewX;
    terry::gray32f_view_t outViewY;

    terry::filter::sobel<std::allocator>(inView, outViewX, outViewY, terry::point2<double>(5, 5),
                                         terry::filter::convolve_option_extend_zero);
}

BOOST_AUTO_TEST_SUITE_END()
