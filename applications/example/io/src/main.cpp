#include <tuttle/common/utils/global.hpp>

#include <tuttle/host/Graph.hpp>

void sam_terminate(void)
{
    TUTTLE_LOG_ERROR("[io example] Sorry, Sam has encountered a fatal error.");
    TUTTLE_LOG_ERROR("[io example] Please report this bug.");
    exit(-1);
}

void sam_unexpected(void)
{
    TUTTLE_LOG_ERROR("[io example] Sorry, Sam has encountered a fatal error.");
    TUTTLE_LOG_ERROR("[io example] Please report this bug.");
    BOOST_THROW_EXCEPTION(
        std::runtime_error("Sorry, Sam has encountered an unexpected exception.\nPlease report this bug."));
}

int main(int argc, char** argv)
{
    tuttle::common::Formatter::get();

    std::set_terminate(&sam_terminate);
    std::set_unexpected(&sam_unexpected);
    try
    {
        using namespace tuttle::host;
        TUTTLE_LOG_INFO("[canny example] Preload plugins");
        core().preload();

        TUTTLE_LOG_INFO(core().getImageEffectPluginCache());

        TUTTLE_LOG_INFO("[canny example] Preload done");

        TUTTLE_LOG_INFO("[canny example] create plugins");
        Graph g;
        //		Graph::Node& read1   = g.createNode( "tuttle.ffmpegreader" );
        Graph::Node& read1 = g.createNode("tuttle.pngreader");
        Graph::Node& read2 = g.createNode("tuttle.dpxreader");
        Graph::Node& read3 = g.createNode("tuttle.exrreader");
        Graph::Node& bitdepth = g.createNode("tuttle.bitdepth");
        Graph::Node& invert1 = g.createNode("tuttle.invert");
        /*Graph::Node& invert2 = */ g.createNode("tuttle.invert");
        /*Graph::Node& invert2 = */ g.createNode("tuttle.invert");
        Graph::Node& invert2 = g.createNode("tuttle.invert");
        Graph::Node& blur1 = g.createNode("tuttle.blur");
        Graph::Node& invert4 = g.createNode("tuttle.invert");
        //	Graph::Node& crop1   = g.createNode( "tuttle.crop" );
        Graph::Node& merge1 = g.createNode("tuttle.merge");
        Graph::Node& bitdepth1 = g.createNode("tuttle.bitdepth");
        Graph::Node& write1 = g.createNode("tuttle.pngwriter");
        Graph::Node& write2 = g.createNode("tuttle.dpxwriter");
        Graph::Node& write3 = g.createNode("tuttle.exrwriter");
        Graph::Node& write4 = g.createNode("tuttle.ffmpegwriter");

        TUTTLE_LOG_INFO("[canny example] set plugins parameters");
        // Setup parameters
        //		read1.getParam( "filename" ).setValue( "data/input1.avi" );
        read1.getParam("filename").setValue("data/input.png");
        read2.getParam("filename").setValue("data/input.dpx");
        read3.getParam("filename").setValue("data/input.exr");
        bitdepth.getParam("outputBitDepth").setValue(3);

        TUTTLE_LOG_VAR(TUTTLE_INFO, bitdepth.getParam("outputBitDepth").getStringValue());

        blur1.getParam("size").setValue(6.5, 15.0);
        //		blur1.getParam( "size" ).setAtIndex( 65.43, 1 );
        //	crop1.getParam( "Down" ).setValue( 400 );
        write1.getParam("filename").setValue("data/output1.png");
        write2.getParam("filename").setValue("data/output2.dpx");
        write3.getParam("filename").setValue("data/output3.exr");
        write4.getParam("filename").setValue("data/output4.avi");

        TUTTLE_LOG_INFO("[canny example] connect plugins");
        g.connect(read1, bitdepth);
        g.connect(bitdepth, invert1);
        g.connect(invert1, invert2);
        g.connect(invert2, blur1);
        g.connect(blur1, write1);
        g.connect(invert1, invert4);
        g.connect(invert4, write2);
        g.connect(invert1, write3);

        g.connect(invert1, bitdepth1);
        g.connect(bitdepth1, merge1.getAttribute("SourceA"));
        g.connect(read3, merge1.getAttribute("SourceB"));
        //	g.connect( merge1, crop1 );
        g.connect(merge1, write4);

        TUTTLE_LOG_INFO("[canny example] process graph");
        std::list<std::string> outputs;
        outputs.push_back(write1.getName());
        //		outputs.push_back( write2.getName() );
        //		outputs.push_back( write3.getName() );
        outputs.push_back(write4.getName());
        g.compute(outputs);
        TUTTLE_LOG_INFO("[canny example] finish");
    }
    catch(tuttle::exception::Common& e)
    {
        TUTTLE_LOG_ERROR("Tuttle Exception : main de sam.");
        TUTTLE_LOG_ERROR(boost::diagnostic_information(e));
        return -1;
    }
    catch(...)
    {
        TUTTLE_LOG_ERROR("Exception ... : main de sam.");
        TUTTLE_LOG_ERROR(boost::current_exception_diagnostic_information());
        return -1;
    }

    return 0;
}
