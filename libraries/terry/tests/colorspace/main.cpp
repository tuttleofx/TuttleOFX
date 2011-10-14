#include <terry/color/colorspace.hpp>
#include <terry/color/colorspace/all.hpp>

#include <iostream>

#define BOOST_TEST_MODULE terry_colorspace_tests
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( terry_colorspace_tests_suite01 )

BOOST_AUTO_TEST_CASE( basics )
{
	terry::color::FullColorParams<terry::color::RGB> rgb_full_params;
	terry::color::FullColorParams<terry::color::HSL> hsl_full_params;
	terry::color::FullColorParams<terry::color::CMYK> cmyk_full_params;
	terry::color::FullColorParams<terry::color::LMS> lms_full_params;
	
//	std::vector<IColorParams*> v = hsl_full_params.getColorspaceVectorView();
	::terry::rgb32f_pixel_t a = terry::get_black<terry::rgb32f_pixel_t>();
	::terry::hsl32f_pixel_t b;
	
	terry::color::color_transformation( rgb_full_params, a, hsl_full_params, b );
	
	BOOST_CHECK_EQUAL(
		terry::color::nbCommonColorspace( hsl_full_params, cmyk_full_params ),
		terry::color::nbCommonColorspace( cmyk_full_params, hsl_full_params ) );
	BOOST_CHECK_EQUAL(
		terry::color::nbCommonColorspace( hsl_full_params, lms_full_params ),
		terry::color::nbCommonColorspace( lms_full_params, hsl_full_params ) );
	
	BOOST_CHECK_EQUAL( terry::color::nbCommonColorspace( hsl_full_params, cmyk_full_params ), std::size_t(2) );
	BOOST_CHECK_EQUAL( terry::color::nbCommonColorspace( hsl_full_params, lms_full_params ), std::size_t(1) );
	
	BOOST_CHECK_EQUAL( std::size_t(terry::color::FullColorParams<terry::color::HSL>::size::value), std::size_t(3) );
//	BOOST_CHECK_EQUAL( std::size_t(terry::color::FullColorParams<terry::color::HSL>::size::value), v.size() );
}


BOOST_AUTO_TEST_SUITE_END()
