#define BOOST_TEST_MODULE plugin_Jpeg2000
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(plugin_Jpeg2000_reader)
std::string pluginName = "tuttle.jpeg2000reader";
std::string filename = "j2k/Bretagne1.j2k";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_Jpeg2000_writer)
std::string pluginName = "tuttle.jpeg2000writer";
std::string filename = "test-jpeg2000.j2k";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()
