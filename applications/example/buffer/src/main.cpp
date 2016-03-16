#include <tuttle/common/utils/global.hpp>

#include <tuttle/host/Graph.hpp>

void sam_terminate(void)
{
    TUTTLE_LOG_ERROR("[buffer example] Sorry, Sam has encountered a fatal error.");
    TUTTLE_LOG_ERROR("[buffer example] Please report this bug.");
    exit(-1);
}

void sam_unexpected(void)
{
    TUTTLE_LOG_ERROR("[buffer example] Sorry, Sam has encountered a fatal error.");
    TUTTLE_LOG_ERROR("[buffer example] Please report this bug.");
    BOOST_THROW_EXCEPTION(
        std::runtime_error("Sorry, Sam has encountered an unexpected exception.\nPlease report this bug."));
}

int main(int argc, char** argv)
{
    std::set_terminate(&sam_terminate);
    std::set_unexpected(&sam_unexpected);
    try
    {
        using namespace tuttle::host;
        TUTTLE_LOG_INFO("[buffer example] Preload plugin");
        // core().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
        // core().getPluginCache().scanPluginFiles();
        core().preload();

        TUTTLE_LOG_INFO(core().getImageEffectPluginCache());

        TUTTLE_LOG_INFO("[buffer example] Preload done");

        TUTTLE_LOG_INFO("[buffer example] Create plugins");
        Graph g;
        //		Graph::Node& read1   = g.createNode( "tuttle.ffmpegreader" );
        Graph::Node& read1 = g.createNode("tuttle.pngreader");
        Graph::Node& read2 = g.createNode("tuttle.oiioreader");
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

        TUTTLE_LOG_INFO("[buffer example] Set plugins parameters");
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

        TUTTLE_LOG_INFO("[buffer example] Connect plugins");
        g.connect(read1, bitdepth);
        g.connect(bitdepth, invert1);
        g.connect(invert1, invert2);
        g.connect(invert2, blur1);
        g.connect(invert1, invert4);

        g.connect(invert1, bitdepth1);
        g.connect(bitdepth1, merge1.getAttribute("A"));
        g.connect(read3, merge1.getAttribute("B"));

        TUTTLE_LOG_INFO("[buffer example] Process");
        std::list<std::string> outputs;
        outputs.push_back(merge1.getName());
        outputs.push_back(invert1.getName());

        memory::MemoryCache outputCache;
        g.compute(outputCache, outputs);

        TUTTLE_LOG_INFO("[buffer example] Finish");
        TUTTLE_LOG_VAR(TUTTLE_INFO, invert1.getName());
        memory::CACHE_ELEMENT img = outputCache.get(invert1.getName(), 0);

        if(!img.get())
        {
            TUTTLE_LOG_ERROR("[buffer example] No buffer...");
            return 1;
        }
        TUTTLE_LOG_INFO("[buffer example] Print Buffer Status");
        ofx::imageEffect::EBitDepth bd = img->getBitDepth();
        ofx::imageEffect::EPixelComponent pc = img->getComponentsType();
        OfxRectI bounds = img->getBounds();

        TUTTLE_LOG_VAR(TUTTLE_INFO, mapBitDepthEnumToString(bd));
        TUTTLE_LOG_VAR(TUTTLE_INFO, mapPixelComponentEnumToString(pc));
        TUTTLE_LOG_VAR4(TUTTLE_INFO, bounds.x1, bounds.y1, bounds.x2, bounds.y2);
        TUTTLE_LOG_VAR(TUTTLE_INFO, img->getClipName());
        // TUTTLE_LOG_VAR( img->getPixelData() );
    }
    catch(tuttle::exception::Common& e)
    {
        TUTTLE_LOG_ERROR("Tuttle Exception : main de sam.");
        TUTTLE_LOG_ERROR(boost::diagnostic_information(e));
    }
    catch(...)
    {
        TUTTLE_LOG_ERROR("Exception ... : main de sam.");
        TUTTLE_LOG_ERROR(boost::current_exception_diagnostic_information());
    }

    return 0;
}
