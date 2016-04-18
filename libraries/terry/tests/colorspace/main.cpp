#include <terry/colorspace/colorspace.hpp>
#include <terry/colorspace/colorspace/all.hpp>

#include <iostream>

#define BOOST_TEST_MODULE terry_colorspace_tests
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(terry_colorspace_tests_suite01)

BOOST_AUTO_TEST_CASE(rgb_to_rgb)
{
    ::terry::color::FullColorParams<terry::color::RGB> rgb_full_params;

    ::terry::rgb32f_pixel_t a = terry::get_black<terry::rgb32f_pixel_t>();
    ::terry::rgb32f_pixel_t b;

    ::terry::color::color_transformation(rgb_full_params, a, rgb_full_params, b);

    // @todo check a == b
}
BOOST_AUTO_TEST_CASE(rgb_to_hsl)
{
    ::terry::color::FullColorParams<terry::color::RGB> rgb_full_params;
    ::terry::color::FullColorParams<terry::color::HSL> hsl_full_params;

    ::terry::rgb32f_pixel_t a = terry::get_black<terry::rgb32f_pixel_t>();
    ::terry::hsl32f_pixel_t b;

    ::terry::color::color_transformation(rgb_full_params, a, hsl_full_params, b);
}

BOOST_AUTO_TEST_CASE(cmyk_to_lms)
{
    ::terry::color::FullColorParams<terry::color::CMYK> cmyk_full_params;
    ::terry::color::FullColorParams<terry::color::LMS> lms_full_params;

    ::terry::cmyk32f_pixel_t a;
    ::terry::lms32f_pixel_t b;

    ::terry::color::color_transformation(cmyk_full_params, a, lms_full_params, b);
}

BOOST_AUTO_TEST_CASE(hsl_to_rgb)
{
    BOOST_CHECK_EQUAL(std::size_t(terry::color::FullColorParams<terry::color::HSL>::size::value), std::size_t(3));
    BOOST_CHECK_EQUAL(std::size_t(terry::color::FullColorParams<terry::color::RGB>::size::value), std::size_t(2));
}

BOOST_AUTO_TEST_SUITE_END()
