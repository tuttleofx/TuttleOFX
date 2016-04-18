#ifndef _TUTTLE_PLUGIN_COLORGRADIENT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADIENT_PROCESS_HPP_

#include "ColorGradientAlgorithm.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradient
{

/**
 * @brief ColorGradient process
 */
template <class View, template <typename> class ColorGradientFunctor>
class ColorGradientProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef ColorGradientFunctor<Pixel> ColorGradientFunctorT;
    typedef typename ColorGradientFunctorT::point_t Point;
    typedef boost::gil::virtual_2d_locator<ColorGradientFunctorT, false> Locator;
    typedef boost::gil::image_view<Locator> ColorGradientVirtualView;

protected:
    ColorGradientPlugin& _plugin;      ///< Rendering plugin
    ColorGradientVirtualView _srcView; ///< Source view

public:
    ColorGradientProcess(ColorGradientPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorGradientProcess.tcc"

#endif
