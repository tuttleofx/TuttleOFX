#ifndef _TUTTLE_PLUGIN_CONSTANT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CONSTANT_PROCESS_HPP_

#include <terry/globals.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <terry/generator/constant.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace constant
{

template <class View>
struct ConstantParams
{
    typedef typename View::value_type Pixel;
    Pixel _color;
};

template <class View>
class ConstantProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename View::point_t Point;
    typedef typename terry::generator::ConstantColorViewFactory<Pixel>::ConstantVirtualView ConstantVirtualView;

protected:
    ConstantPlugin& _plugin; ///< Rendering plugin

    ConstantParams<View> _params;

public:
    ConstantProcess(ConstantPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    ConstantParams<View> getParams();

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ConstantProcess.tcc"

#endif
