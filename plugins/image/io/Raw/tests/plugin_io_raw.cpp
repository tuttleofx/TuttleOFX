#define BOOST_TEST_MODULE plugin_Raw
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(plugin_Raw_reader)
std::string pluginName = "tuttle.rawreader";
std::string filename = "raw/RAW_CANON_10D.CRW";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(plugin_Raw_writer)
std::string pluginName = "tuttle.rawwriter";
std::string filename = "test-raw";
//#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()
