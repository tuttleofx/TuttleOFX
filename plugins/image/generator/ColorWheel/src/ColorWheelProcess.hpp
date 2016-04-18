#ifndef _TUTTLE_PLUGIN_COLORWHEEL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORWHEEL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <terry/globals.hpp>
#include <terry/generator/colorwheel.hpp>
#include <terry/generator/rainbow.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorWheel
{

/**
 * @brief ColorWheel process
 *
 */
template <class View>
class ColorWheelProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef terry::generator::ColorWheelFunctor<Pixel> ColorWheelFunctorT;
    typedef typename ColorWheelFunctorT::point_t ColorWheelPoint;
    typedef boost::gil::virtual_2d_locator<ColorWheelFunctorT, false> ColorWheelLocator;
    typedef boost::gil::image_view<ColorWheelLocator> ColorWheelVirtualView;

    typedef terry::generator::RainbowFunctor<Pixel> RainbowFunctorT;
    typedef typename RainbowFunctorT::point_t RainbowPoint;
    typedef boost::gil::virtual_2d_locator<RainbowFunctorT, false> RainbowLocator;
    typedef boost::gil::image_view<RainbowLocator> RainbowVirtualView;

protected:
    ColorWheelPlugin& _plugin;                ///< Rendering plugin
    ColorWheelVirtualView _srcColorWheelView; ///< Source view
    RainbowVirtualView _srcRainbowView;       ///< Source view

    ColorWheelProcessParams _params;

public:
    ColorWheelProcess(ColorWheelPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorWheelProcess.tcc"

#endif
