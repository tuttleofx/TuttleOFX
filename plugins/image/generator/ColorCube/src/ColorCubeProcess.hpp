#ifndef _TUTTLE_PLUGIN_COLORCUBE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORCUBE_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <terry/generator/colorcube.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCube
{

/**
 * @brief ColorCube process
 *
 */
template <class View>
class ColorCubeProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef terry::generator::ColorCubeFunctor<Pixel> ColorCubeFunctorT;
    typedef typename ColorCubeFunctorT::point_t ColorCubePoint;
    typedef boost::gil::virtual_2d_locator<ColorCubeFunctorT, false> ColorCubeLocator;
    typedef boost::gil::image_view<ColorCubeLocator> ColorCubeVirtualView;

protected:
    ColorCubePlugin& _plugin;               ///< Rendering plugin
    ColorCubeVirtualView _srcColorCubeView; ///< Source view

    ColorCubeProcessParams _params; ///< parameters

public:
    ColorCubeProcess(ColorCubePlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorCubeProcess.tcc"

#endif
