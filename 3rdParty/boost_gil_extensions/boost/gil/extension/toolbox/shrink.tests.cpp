#include <boost/test/included/unit_test.hpp>

#include <shrink.hpp>
#include <fill.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

//g++ shrink.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

struct test1
{
	void operator()()
	{
		int width = 200;
		int height = 200;
		unsigned char* buffer = new unsigned char[width * height * 3];
		memset(buffer, 255, width * height * 3);	
		rgb8_view_t view = interleaved_view(width,height,(rgb8_pixel_t*)buffer,width*3);

		rgb8_view_t::value_type black(0,0,0);
		layer::fill<rgb8_view_t> fill(black);
		layer::shrink<rgb8_view_t> shrink(fill,0.20,0.30,0.20,0.30);
		shrink(view);
		
		FILE* fd = fopen("shrink.ppm", "wb");
		fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
		fwrite(buffer, 1, width*height*3, fd);
		fclose(fd);
		delete [] buffer;
	}
};

test_suite* init_unit_test_suite( int argc, char** argv)
{
	test_suite* test= BOOST_TEST_SUITE( "rounded tests" );
	test->add( BOOST_TEST_CASE(test1()), 0);
	return test;
}
