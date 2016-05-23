#include <boost/test/included/unit_test.hpp>

#include <fill.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ fill.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        layer::fill<rgb8_view_t> fill(rgb8_view_t::value_type(0, 255, 0));
        layer::fill_horizontal<rgb8_view_t> horz(fill, 20, 40, 0, 100);
        horz(view);

        FILE* fd = fopen("fill.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("fill tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    return test;
}
