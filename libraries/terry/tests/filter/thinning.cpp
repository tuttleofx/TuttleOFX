#include <terry/globals.hpp>
#include <terry/filter/thinning.hpp>

#include <iostream>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(terry_filter_thinning_tests_suite01)

BOOST_AUTO_TEST_CASE(thinning)
{
    // empty input/output, the test only check the compilation
    terry::rgb32f_view_t inView;
    terry::rgb32f_view_t tmpView;
    terry::rgb32f_view_t outView;

    terry::filter::applyThinning(inView, tmpView, outView);
}

BOOST_AUTO_TEST_SUITE_END()
