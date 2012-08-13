#define BOOST_TEST_MODULE plugin_Dpx
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_Dpx_reader )
std::string pluginName = "tuttle.dpxreader";
std::string filename = "dpx/flowers-1920x1080-RGB-10.dpx";
//#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( plugin_Dpx_writer )
std::string pluginName = "tuttle.dpxwriter";
std::string filename = "test-png.png";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()
