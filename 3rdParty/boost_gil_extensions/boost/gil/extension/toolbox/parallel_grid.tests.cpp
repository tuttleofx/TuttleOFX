#include <boost/test/included/unit_test.hpp>

#include <parallel_grid.hpp>
#include <fill.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

//g++ parallel_grid.tests.cpp -I /home/scott/project/boost_1_36_0 -I . -ltbb -lpthread

struct test1
{
	void operator()()
	{
		int width = 200;
		int height = 200;
		unsigned char* buffer = new unsigned char[width * height * 3];
		memset(buffer, 255, width * height * 3);	
		rgb8_view_t view = interleaved_view(width,height,(rgb8_pixel_t*)buffer,width*3);

		typedef boost::function<void (rgb8_view_t&)> layer_t;
		typedef layer::fill<rgb8_view_t> fill_t;

		layer_t a[] = 
		{
			fill_t(rgb8_view_t::value_type(255,0,255)),
			fill_t(rgb8_view_t::value_type(255,0,0)),
			fill_t(rgb8_view_t::value_type(0,0,255)),
			fill_t(rgb8_view_t::value_type(255,0,0)),
			fill_t(rgb8_view_t::value_type(100,0,255)),
			fill_t(rgb8_view_t::value_type(100,0,100)),
		};

		tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
		init.initialize(1);

		layer::parallel_grid<rgb8_view_t> grid(a,6,3);
		grid(view);
		
		FILE* fd = fopen("parallel_grid.ppm", "wb");
		fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
		fwrite(buffer, 1, width*height*3, fd);
		fclose(fd);
		delete [] buffer;
	}
};

struct test2
{
	void operator()()
	{
		sections curr(100,3);

		double a[] = {34,33,33};

		int n = 0;
		for(; curr; ++curr, n++)
		{
			BOOST_CHECK(a[n] == *curr);
		}
	}
};

test_suite* init_unit_test_suite( int argc, char** argv)
{
	test_suite* test= BOOST_TEST_SUITE( "border tests" );
	test->add( BOOST_TEST_CASE(test1()), 0);
	test->add( BOOST_TEST_CASE(test2()), 0);
	return test;
}
