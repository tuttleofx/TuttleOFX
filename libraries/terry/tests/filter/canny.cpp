#include <terry/globals.hpp>
#include <terry/filter/canny.hpp>

#include <iostream>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( terry_filter_canny_tests_suite01 )

BOOST_AUTO_TEST_CASE( canny )
{
	// empty input/output, the test only check the compilation
	terry::rgb32f_view_t inView;
	terry::rgb32f_view_t cannyView;

/*
	terry::filter::canny( inView,
		sobelViewX, sobelViewY, cannyView,
		point2<double>(5, 5),
		terry::filter::canny::convolve_boundary_,
		);
*/
}


BOOST_AUTO_TEST_SUITE_END()
