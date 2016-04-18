#ifndef _TUTTLE_PLUGIN_COLORBARS_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORBARS_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <terry/generator/colorbars.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorBars
{

/**
 * @brief ColorBars process
 *
 */
template <class View>
class ColorBarsProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef terry::generator::ColorBarsFunctor<Pixel> ColorBarsFunctorT;
    typedef typename ColorBarsFunctorT::point_t Point;
    typedef boost::gil::virtual_2d_locator<ColorBarsFunctorT, false> Locator;
    typedef boost::gil::image_view<Locator> ColorBarsVirtualView;

protected:
    ColorBarsPlugin& _plugin;      ///< Rendering plugin
    ColorBarsVirtualView _srcView; ///< Source view

public:
    ColorBarsProcess(ColorBarsPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorBarsProcess.tcc"

#endif
