#include <tuttle/host/Graph.hpp>
#include <tuttle/host/Node.hpp>

int main(int argc, char** argv)
{
    using namespace tuttle::host;

    core().preload();

    const bool res =
        compute(list_of(NodeInit("tuttle.pngreader").setParam("filename", "data/input.png").setParamExp("bitDepth", "32f"))(
            NodeInit("tuttle.pngwriter").setParam("filename", "out.png")));

    if(!res)
        return -1;
    return 0;
}
