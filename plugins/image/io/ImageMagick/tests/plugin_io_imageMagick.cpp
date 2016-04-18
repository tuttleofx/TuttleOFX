#define BOOST_TEST_MODULE plugin_ImageMagick
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(plugin_ImageMagick_jpg_reader)
std::string pluginName = "tuttle.imagemagickreader";
std::string filename = "jpeg/BLU.JPG";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_ImageMagick_ai_reader)
std::string pluginName = "tuttle.imagemagickreader";
std::string filename = "ai/GRACoL7_press_bar_005.ai";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_ImageMagick_png_reader)
std::string pluginName = "tuttle.imagemagickreader";
std::string filename = "png/Gradient-8bit.png";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_ImageMagick_tif_reader)
std::string pluginName = "tuttle.imagemagickreader";
std::string filename = "tif/test-ramp.tif";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_ImageMagick_writer)
std::string pluginName = "tuttle.imagemagickwriter";
std::string filename = "test-imagemagick.jpg";
//#include <tuttle/test/io/writer.hpp>

BOOST_AUTO_TEST_SUITE_END()
