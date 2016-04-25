#include "ColorCubeAlgorithm.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCube
{

template <class View>
ColorCubeProcess<View>::ColorCubeProcess(ColorCubePlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ColorCubeProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace terry;
    boost::function_requires<PixelLocatorConcept<ColorCubeLocator> >();
    gil_function_requires<StepIteratorConcept<typename ColorCubeLocator::x_iterator> >();

    ImageGilProcessor<View>::setup(args);
    _params = _plugin.getProcessParams();

    OfxRectD rod = _plugin._clipDst->getCanonicalRod(args.time, args.renderScale);

    int xshift = 0.0;
    int yshift = 0.0;

    ColorCubePoint dims(rod.x2 - rod.x1, rod.y2 - rod.y1);
    float size = std::min(dims.x, dims.y);

    if(size == dims.x)
        yshift = -boost::numeric_cast<int>((dims.y - dims.x) * 0.5);
    else
        xshift = -boost::numeric_cast<int>((dims.x - dims.y) * 0.5);

    ColorCubePoint tileSize(size, size);

    // create a squared color Cube
    ColorCubeVirtualView colorCube(ColorCubePoint(size, size), ColorCubeLocator(ColorCubePoint(0, 0), ColorCubePoint(1, 1),
                                                                                ColorCubeFunctorT(tileSize, _params.step)));
    // create a subview depending on the image ratio
    _srcColorCubeView =
        subimage_view<>(colorCube, xshift, yshift, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y));
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorCubeProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename ColorCubeVirtualView::x_iterator src_it = this->_srcColorCubeView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            (*dst_it) = (*src_it);
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
