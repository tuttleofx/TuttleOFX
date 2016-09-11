#define BOOST_TEST_MODULE tuttle_asynchronous
#include <tuttle/test/main.hpp>

#include <tuttle/host/ThreadEnv.hpp>
#include <tuttle/host/Graph.hpp>

#include <iostream>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE(asynchronous_suite)

BOOST_AUTO_TEST_CASE(computeAsynchronous)
{
    TUTTLE_LOG_INFO("--> PLUGINS CREATION");
    Graph g;
    Graph::Node& read1 = g.createNode("tuttle.oiioreader");
    Graph::Node& read2 = g.createNode("tuttle.oiioreader");
    Graph::Node& invert1 = g.createNode("tuttle.invert");
    Graph::Node& invert2 = g.createNode("tuttle.invert");
    Graph::Node& invert3 = g.createNode("tuttle.invert");
    Graph::Node& invert4 = g.createNode("tuttle.invert");
    //	Graph::Node& merge1 = g.createNode( "tuttle.merge" );
    Graph::Node& write1 = g.createNode("tuttle.pngwriter");
    Graph::Node& write4 = g.createNode("tuttle.pngwriter");
    Graph::Node& write2 = g.createNode("tuttle.oiiowriter");
    Graph::Node& write3 = g.createNode("tuttle.exrwriter");

    TUTTLE_LOG_INFO("--> PLUGINS CONFIGURATION");
    // Setup parameters
    read1.getParam("filename").setValue("TuttleOFX-data/image/jpeg/GRN.JPG");
    read2.getParam("filename").setValue("TuttleOFX-data/image/jpeg/RED.JPG");
    read1.getParam("bitDepth").setValue(3);
    read2.getParam("bitDepth").setValue(3);
    //	crop1.getParam( "Down" ).setValue( 400 );
    write1.getParam("filename").setValue(".tests/processGraph/output1.png");
    write2.getParam("filename").setValue(".tests/processGraph/output2.jpg");
    write3.getParam("filename").setValue(".tests/processGraph/output3.exr");
    write4.getParam("filename").setValue(".tests/processGraph/output4.png");

    TUTTLE_LOG_INFO("-------- GRAPH CONNECTION --------");
    TUTTLE_LOG_TRACE("connect");
    g.connect(read1, invert1);
    TUTTLE_LOG_TRACE("connect");
    //		g.connect( invert1, bitdepth );
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert1, invert2);
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert2, invert3);
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert3, write1);
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert1, invert4);
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert4, write2);
    TUTTLE_LOG_TRACE("connect");
    g.connect(invert1, write3);

    TUTTLE_LOG_INFO("-------- GRAPH CONNECT CLIPS --------");
    //	g.connect( invert1, merge1.getAttribute( "A" ) );
    //	g.connect( read2, merge1.getAttribute( "B" ) );
    //	g.connect( merge1, write4 );
    g.connect(invert1, write4);

    TUTTLE_LOG_INFO("-------- SET GRAPH OUTPUTS --------");
    std::list<std::string> outputs;
    outputs.push_back(write1.getName());
    outputs.push_back(write2.getName());
    outputs.push_back(write3.getName());
    outputs.push_back(write4.getName());

    TUTTLE_LOG_INFO("-------- GRAPH PROCESSING --------");
    ThreadEnv env;
    env.getComputeOptions().setTimeRange(0, 15);
    env.compute(g, outputs);
    env.abort();

    env.join();

    // the process result is not successful, because it has been aborted.
    BOOST_CHECK(!env.getResult());

    TUTTLE_LOG_INFO("----------------- DONE -----------------");
}

BOOST_AUTO_TEST_SUITE_END()
