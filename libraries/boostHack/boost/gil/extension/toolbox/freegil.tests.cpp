#include <boost/test/included/unit_test.hpp>

#include <freegil.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

// g++ freegil.tests.cpp -I /home/scott/project/boost_1_36_0 -I . -I /usr/include/freetype2 -lfreetype

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view2 = interleaved_view(width, height, (rgb8_pixel_t*)buffer, width * 3);
        rgb8_view_t view = subimage_view(view2, 0, 0, width - 5, height - 5);

        const char* fontnames[] = {"/Mini/MINIHHB_.TTF", "/Mini/MINXB___.TTF"};
        int size = sizeof(fontnames) / sizeof(const char*);

        char fontpath[255];
        strcpy(fontpath, getenv("FONT_PATH") ? getenv("FONT_PATH") : "/home/scott/project/fonts/");

        char** fonts = (char**)malloc(size * sizeof(char*));
        for(int n = 0; n < size; ++n)
        {
            char tmp[255];
            sprintf(tmp, "%s%s", fontpath, fontnames[n]);
            fonts[n] = (char*)malloc((strlen(tmp) + 1) * sizeof(char));
            sprintf(fonts[n], "%s", tmp);
        }

        FT_Library library;
        FTC_Manager manager;
        FT_Init_FreeType(&library);
        FTC_Manager_New(library, 0, 0, 0, face_requester, fonts, &manager);

        typedef layer::text<rgb8_view_t> text_t;
        rgb8_view_t::value_type black(0, 0, 0);
        text_t text(manager, "Hello", black, 0, 12);
        text(view);

        for(int n = 0; n < size; ++n)
            free(fonts[n]);
        free(fonts);

        FTC_Manager_Done(manager);
        FT_Done_FreeType(library);

        FILE* fd = fopen("freegil.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width * height * 3, fd);
        fclose(fd);
        delete[] buffer;
    }
};

test_suite* init_unit_test_suite(int argc, char** argv)
{
    test_suite* test = BOOST_TEST_SUITE("freegil tests");
    test->add(BOOST_TEST_CASE(test1()), 0);
    return test;
}
