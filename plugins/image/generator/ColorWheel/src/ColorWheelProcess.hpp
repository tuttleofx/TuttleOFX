#ifndef _TUTTLE_PLUGIN_COLORWHEEL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORWHEEL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <terry/globals.hpp>
#include <terry/generator/colorwheel.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace colorWheel {

/**
 * @brief ColorWheel process
 *
 */
template<class View>
class ColorWheelProcess : public ImageGilProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef terry::generator::ColorWheelFunctor<Pixel> ColorWheelFunctorT;
	typedef typename ColorWheelFunctorT::point_t Point;
	typedef boost::gil::virtual_2d_locator<ColorWheelFunctorT, false> Locator;
	typedef boost::gil::image_view<Locator> ColorWheelVirtualView;
	
protected:
	ColorWheelPlugin&     _plugin;   ///< Rendering plugin
	ColorWheelVirtualView _srcView;  ///< Source view

public:
	ColorWheelProcess( ColorWheelPlugin& effect );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorWheelProcess.tcc"

#endif
