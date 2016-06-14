#define BOOST_TEST_MODULE plugin_OpenImageIO
#include <tuttle/test/main.hpp>

#define PLUGIN_ID "OPEN IMAGE IO"

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

//// READER ////

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_dpx_reader)
std::string pluginName = "tuttle.oiioreader";
std::string filename = "dpx/flowers-1920x1080-RGB-10.dpx";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_exr_reader)
std::string pluginName = "tuttle.oiioreader";
std::string filename = "openexr/TestImages/GammaChart.exr";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_jpg_reader)
std::string pluginName = "tuttle.oiioreader";
std::string filename = "jpeg/BLU.JPG";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( plugin_OpenImageIO_j2k_reader )
std::string pluginName = "tuttle.oiioreader";
std::string filename = "j2k/Bretagne1.j2k";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( plugin_OpenImageIO_jp2_reader )
std::string pluginName = "tuttle.oiioreader";
std::string filename = "jp2/relax.jp2";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_png_reader)
std::string pluginName = "tuttle.oiioreader";
std::string filename = "png/Gradient-8bit.png";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_tif_reader)
std::string pluginName = "tuttle.oiioreader";
std::string filename = "tif/test-ramp.tif";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

//// WRITER ////

BOOST_AUTO_TEST_SUITE( plugin_OpenImageIO_dpx_writer )
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.dpx";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_exr_writer)
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.exr";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_jpg_writer)
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.jpg";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( plugin_OpenImageIO_j2k_writer )
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.j2k";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( plugin_OpenImageIO_jp2_writer )
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.jp2";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_png_writer)
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.png";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_OpenImageIO_tif_writer)
std::string pluginName = "tuttle.oiiowriter";
std::string filename = "test-oiio.tif";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()
