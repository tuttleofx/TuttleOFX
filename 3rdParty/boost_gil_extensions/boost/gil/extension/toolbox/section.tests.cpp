#include <boost/test/included/unit_test.hpp>

#include <sections.hpp>
#include <fill.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

//g++ section.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

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
		typedef std::pair<double, layer_t> pair_t;
		typedef layer::fill<rgb8_view_t> fill_t;

		pair_t a[] = 
		{
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,0))),
			pair_t(0,fill_t(rgb8_view_t::value_type(0,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,0))),
			pair_t(30,fill_t(rgb8_view_t::value_type(100,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(100,0,100))),
		};

		layer::rows<rgb8_view_t> rows(a,6,5);
		rows(view);
		
		FILE* fd = fopen("sections.test1.ppm", "wb");
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
		int width = 200;
		int height = 200;
		unsigned char* buffer = new unsigned char[width * height * 3];
		memset(buffer, 255, width * height * 3);	
		rgb8_view_t view = interleaved_view(width,height,(rgb8_pixel_t*)buffer,width*3);

		typedef boost::function<void (rgb8_view_t&)> layer_t;
		typedef std::pair<double, layer_t> pair_t;
		typedef layer::fill<rgb8_view_t> fill_t;

		pair_t a[] = 
		{
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,0))),
			pair_t(0,fill_t(rgb8_view_t::value_type(0,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(255,0,0))),
			pair_t(30,fill_t(rgb8_view_t::value_type(100,0,255))),
			pair_t(30,fill_t(rgb8_view_t::value_type(100,0,100))),
		};

		layer::columns<rgb8_view_t> rows(a,6,5);
		rows(view);
		
		FILE* fd = fopen("sections.test2.ppm", "wb");
		fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
		fwrite(buffer, 1, width*height*3, fd);
		fclose(fd);
		delete [] buffer;
	}
};

test_suite* init_unit_test_suite( int argc, char** argv)
{
	test_suite* test= BOOST_TEST_SUITE( "sections tests" );
	test->add( BOOST_TEST_CASE(test1()), 0);
	test->add( BOOST_TEST_CASE(test2()), 0);
	return test;
}
