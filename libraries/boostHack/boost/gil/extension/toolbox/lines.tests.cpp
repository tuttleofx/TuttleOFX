#include <boost/test/included/unit_test.hpp>

#include <lines.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ lines.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

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

        layer::uniform_horizontal_lines<rgb8_view_t> lines(rgb8_view_t::value_type(0, 0, 0), 10);
        lines(view);

        FILE* fd = fopen("lines1.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

struct test2
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        typedef boost::function<void(rgb8_view_t&)> layer_t;

        layer::uniform_vertical_lines<rgb8_view_t> lines(rgb8_view_t::value_type(0, 0, 0), 10);
        lines(view);

        FILE* fd = fopen("lines2.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

struct test3
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        typedef boost::function<void(rgb8_view_t&)> layer_t;

        layer::horizontal_line<rgb8_view_t> line(rgb8_view_t::value_type(0, 0, 0), 30, 0, 100);
        line(view);

        FILE* fd = fopen("lines3.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

struct test4
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        typedef boost::function<void(rgb8_view_t&)> layer_t;

        layer::vertical_lines<rgb8_view_t> line(rgb8_view_t::value_type(0, 0, 0), 10, 20);
        line(view);

        FILE* fd = fopen("lines4.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("lines tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    test->add(BOOST_TEST_CASE(test2()), 0);
    test->add(BOOST_TEST_CASE(test3()), 0);
    test->add(BOOST_TEST_CASE(test4()), 0);
    return test;
}
