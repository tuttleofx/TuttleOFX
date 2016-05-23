#ifndef _TUTTLE_PLUGIN_WARP_PROCESS_HPP_
#define _TUTTLE_PLUGIN_WARP_PROCESS_HPP_

#include "TPS/tps.hpp"

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace warp
{

/**
 * @brief Warp process
 *
 */
template <class View>
class WarpProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef WarpPlugin::Scalar Scalar;
    typedef typename View::point_t Point;
    typedef typename View::coord_t Coord;
    typedef typename terry::image_from_view<View>::type Image;

protected:
    WarpPlugin& _plugin; ///< Rendering plugin

    OFX::Clip* _clipSrcB; ///< Source B
    boost::scoped_ptr<OFX::Image> _srcB;
    OfxRectI _srcBPixelRod;
    View _srcBView;

    WarpProcessParams<Scalar> _params; ///< parameters
    TPS_Morpher<Scalar> _tpsA;
    TPS_Morpher<Scalar> _tpsB;

public:
    WarpProcess(WarpPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "WarpProcess.tcc"

#endif
