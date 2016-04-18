#include <boost/test/included/unit_test.hpp>

#include <layers.hpp>
#include <fill.hpp>
#include <shrink.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ layers.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        typedef boost::function<void(rgb8_view_t&)> layer_t;

        layer_t a[] = {
            layer::shrink<rgb8_view_t>(layer::fill<rgb8_view_t>(rgb8_view_t::value_type(255, 0, 255)), 5, 5, 5, 5),
            layer::shrink<rgb8_view_t>(layer::fill<rgb8_view_t>(rgb8_view_t::value_type(255, 0, 0)), 20, 20, 20, 20)};

        layer::layers<rgb8_view_t> layer(a, 2);
        layer(view);

        FILE* fd = fopen("layers.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("rounded tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    return test;
}
