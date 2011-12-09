#include <terry/globals.hpp>
#include <terry/filter/localMaxima.hpp>

#include <iostream>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( terry_filter_localMaxima )

BOOST_AUTO_TEST_CASE( localMaxima )
{
	// empty input/output, the test only check the compilation
	terry::rgb32f_view_t inView;
	terry::rgb32f_view_t outView;

	terry::filter::applyLocalMaxima( inView, outView );
}


BOOST_AUTO_TEST_SUITE_END()
