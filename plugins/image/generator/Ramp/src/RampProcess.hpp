#ifndef _TUTTLE_PLUGIN_RAMP_PROCESS_HPP_
#define _TUTTLE_PLUGIN_RAMP_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <terry/globals.hpp>
#include <terry/generator/ramp.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace ramp
{

template <class View>
struct RampProcessParams
{
    typedef typename View::value_type Pixel;
    bool direction;
    bool color;
    Pixel colorStart;
    Pixel colorEnd;
};

/**
 * @brief Ramp process
 *
 */
template <class View>
class RampProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef terry::generator::RampFunctor<Pixel> RampFunctorT;
    typedef typename RampFunctorT::point_t Point;
    typedef boost::gil::virtual_2d_locator<RampFunctorT, false> Locator;
    typedef boost::gil::image_view<Locator> RampVirtualView;

protected:
    RampPlugin& _plugin;      ///< Rendering plugin
    RampVirtualView _srcView; ///< Source view

public:
    RampProcess(RampPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    RampProcessParams<View> getProcessParams() const;

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "RampProcess.tcc"

#endif
