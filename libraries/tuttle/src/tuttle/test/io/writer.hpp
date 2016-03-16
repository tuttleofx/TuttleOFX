#include <iostream>

#include <boost/filesystem/path.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;
namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_CASE(process_writer)
{
    TUTTLE_LOG_INFO("******** PROCESS WRITER " << pluginName << " ********");
    Graph g;

    TUTTLE_LOG_INFO("-------- PLUGINS CREATION --------");
    Graph::Node& constant = g.createNode("tuttle.constant");
    Graph::Node& writer = g.createNode(pluginName);

    TUTTLE_LOG_INFO("-------- PLUGINS CONFIGURATION --------");

    constant.getParam("width").setValue(500);
    constant.getParam("height").setValue(500);

    std::string tuttleOFXData = "TuttleOFX-data";
    if(const char* env_test_data = std::getenv("TUTTLE_TEST_DATA"))
    {
        tuttleOFXData = env_test_data;
    }

    const std::string pluginFilename = (bfs::path(tuttleOFXData) / "image" / filename).string();
    writer.getParam("filename").setValue(pluginFilename);

    TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
    g.connect(constant, writer);

    TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
    boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
    memory::MemoryCache outputCache;
    g.compute(outputCache, writer);
    boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

    TUTTLE_LOG_INFO("Process took: " << t2a - t1a);
    TUTTLE_LOG_INFO(outputCache);

    memory::CACHE_ELEMENT imgRes = outputCache.get(writer.getName(), 0);

    TUTTLE_LOG_VAR(TUTTLE_INFO, imgRes->getROD());
    BOOST_CHECK_EQUAL(imgRes->getROD().x1, 0);
    BOOST_CHECK_EQUAL(imgRes->getROD().y1, 0);
    BOOST_CHECK_EQUAL(imgRes->getROD().x2, 500);
    BOOST_CHECK_EQUAL(imgRes->getROD().y2, 500);

    TUTTLE_LOG_VAR(TUTTLE_INFO, imgRes->getBounds());
    BOOST_CHECK_EQUAL(imgRes->getBounds().x1, 0);
    BOOST_CHECK_EQUAL(imgRes->getBounds().y1, 0);
    BOOST_CHECK_EQUAL(imgRes->getBounds().x2, 500);
    BOOST_CHECK_EQUAL(imgRes->getBounds().y2, 500);
}

BOOST_AUTO_TEST_CASE(process_unconnected)
{
    TUTTLE_LOG_INFO("******** PROCESS WRITER " << pluginName << " UNCONNECTED ********");
    Graph g;

    TUTTLE_LOG_INFO("--> PLUGINS CREATION");
    Graph::Node& write = g.createNode(pluginName);

    TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
    std::string filename = "data/no-such-file";
    write.getParam("filename").setValue(filename);

    TUTTLE_LOG_INFO("---> GRAPH PROCESSING");
    BOOST_REQUIRE_THROW(g.compute(write), boost::exception);
}
