#include <boost/test/included/unit_test.hpp>

#include <border.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ border.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        layer::raised_border<rgb8_view_t> border(rgb8_view_t::value_type(220, 220, 220),
                                                 rgb8_view_t::value_type(50, 50, 50));
        border(view);

        FILE* fd = fopen("border.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("border tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    return test;
}
