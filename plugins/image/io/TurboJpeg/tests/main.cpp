#define BOOST_TEST_MODULE plugin_TurboJpeg
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_TurboJpeg_reader )
std::string pluginName = "tuttle.turbojpegreader";
std::string filename = "jpeg/BLU.JPG";
#include <tuttle/test/test_io_reader.hpp>
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( plugin_TurboJpeg_writer )
std::string pluginName = "tuttle.turbojpegwriter";
std::string filename = "test-jpeg.jpg";
#include <tuttle/test/test_io_writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

