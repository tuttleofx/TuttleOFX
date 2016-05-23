#include <boost/test/included/unit_test.hpp>

#include <wuline.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ wuline.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);

        double phase = 0;
        for(double theta = phase; theta < 360 + phase; theta += 10)
        {
            int x = (100.0 * cos(theta * 3.14 / 180.0) + 100);
            int y = (-100 * sin(theta * 3.14 / 180.0) + 100);
            wuline(view, rgb8_pixel_t(0, 0, 0), x, y, 100, 100);
        }

        FILE* fd = fopen("wuline.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("wuline tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    return test;
}
