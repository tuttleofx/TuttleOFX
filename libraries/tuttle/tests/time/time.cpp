#define BOOST_TEST_MODULE tuttle_time
#include <tuttle/test/main.hpp>

#include <tuttle/host/Graph.hpp>

#include <iostream>

//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(time_tests_suite01)

BOOST_AUTO_TEST_CASE(time_shift_offset_null)
{
    TUTTLE_LOG_INFO("******** PROCESS TIMESHIFT ********");
    try
    {
        TUTTLE_LOG_INFO("--> PLUGINS CREATION");

        Graph g;
        Graph::Node& read1 = g.createNode("tuttle.oiioreader");
        // Graph::Node& invert1 = g.createNode( "tuttle.invert" );
        Graph::Node& timeshift1 = g.createNode("tuttle.timeshift");
        Graph::Node& write1 = g.createNode("tuttle.oiiowriter");

        TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
        read1.getParam("filename").setValue("TuttleOFX-data/image/jpeg/MARS@.JPG");
        timeshift1.getParam("offset").setValue(0);
        write1.getParam("filename").setValue(".tests/output_####.jpg");

        TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
        g.connect(read1, timeshift1);
        g.connect(timeshift1, write1);

        TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
        std::list<std::string> outputs;
        outputs.push_back(write1.getName());
        // computing at time 1 with an offset of 0, it require the frame 1
        g.compute(outputs, ComputeOptions(1));
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information() << std::endl;
        BOOST_FAIL("Exception");
    }
}

BOOST_AUTO_TEST_CASE(time_shift_positive_offset)
{
    TUTTLE_LOG_INFO("******** PROCESS TIMESHIFT ********");
    try
    {
        TUTTLE_LOG_INFO("--> PLUGINS CREATION");

        Graph g;
        Graph::Node& read1 = g.createNode("tuttle.oiioreader");
        // Graph::Node& invert1 = g.createNode( "tuttle.invert" );
        Graph::Node& timeshift1 = g.createNode("tuttle.timeshift");
        Graph::Node& write1 = g.createNode("tuttle.oiiowriter");

        TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
        read1.getParam("filename").setValue("TuttleOFX-data/image/jpeg/MARS@.JPG");
        timeshift1.getParam("offset").setValue(1);
        write1.getParam("filename").setValue(".tests/output_####.jpg");

        TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
        g.connect(read1, timeshift1);
        g.connect(timeshift1, write1);

        TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
        std::list<std::string> outputs;
        outputs.push_back(write1.getName());
        // computing at time 2 with an offset of 1, it require the frame 1
        g.compute(outputs, ComputeOptions(2));
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information() << std::endl;
        BOOST_FAIL("Exception");
    }
}

BOOST_AUTO_TEST_CASE(time_shift_negative_offset)
{
    TUTTLE_LOG_INFO("******** PROCESS TIMESHIFT ********");
    try
    {
        TUTTLE_LOG_INFO("--> PLUGINS CREATION");

        Graph g;
        Graph::Node& read1 = g.createNode("tuttle.oiioreader");
        // Graph::Node& invert1 = g.createNode( "tuttle.invert" );
        Graph::Node& timeshift1 = g.createNode("tuttle.timeshift");
        Graph::Node& write1 = g.createNode("tuttle.oiiowriter");

        TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
        read1.getParam("filename").setValue("TuttleOFX-data/image/jpeg/MARS@.JPG");
        timeshift1.getParam("offset").setValue(3);
        write1.getParam("filename").setValue(".tests/output_####.jpg");

        TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
        g.connect(read1, timeshift1);
        g.connect(timeshift1, write1);

        TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
        std::list<std::string> outputs;
        outputs.push_back(write1.getName());
        // computing at time 3 with an offset of -2, it require the frame 1
        g.compute(outputs, ComputeOptions(4));
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information() << std::endl;
        BOOST_FAIL("Exception");
    }
}

BOOST_AUTO_TEST_CASE(time_shift_negative_offset_forceIdentity)
{
    TUTTLE_LOG_INFO("******** PROCESS TIMESHIFT ********");
    try
    {
        TUTTLE_LOG_INFO("--> PLUGINS CREATION");

        Graph g;
        Graph::Node& read1 = g.createNode("tuttle.oiioreader");
        // Graph::Node& invert1 = g.createNode( "tuttle.invert" );
        Graph::Node& timeshift1 = g.createNode("tuttle.timeshift");
        Graph::Node& write1 = g.createNode("tuttle.oiiowriter");

        TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
        read1.getParam("filename").setValue("TuttleOFX-data/image/jpeg/MARS@.JPG");
        timeshift1.getParam("offset").setValue(3);
        write1.getParam("filename").setValue(".tests/output_####.jpg");

        TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
        g.connect(read1, timeshift1);
        g.connect(timeshift1, write1);

        TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
        std::list<std::string> outputs;
        outputs.push_back(write1.getName());
        // computing at time 3 with an offset of -2, it require the frame 1
        g.compute(outputs, ComputeOptions(4).setForceIdentityNodesProcess());
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information() << std::endl;
        BOOST_FAIL("Exception");
    }
}

BOOST_AUTO_TEST_SUITE_END()
