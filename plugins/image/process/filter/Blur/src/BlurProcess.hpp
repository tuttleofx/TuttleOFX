#ifndef _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace blur
{

/**
 * @brief Blur process
 *
 */
template <class View>
class BlurProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef float Scalar;
    typedef typename View::value_type Pixel;
    typedef typename View::point_t Point;
    typedef typename View::coord_t Coord;
    typedef typename terry::image_from_view<View>::type Image;

protected:
    BlurPlugin& _plugin; ///< Rendering plugin

    BlurProcessParams<Scalar> _params; ///< user parameters

public:
    BlurProcess(BlurPlugin& effect);

    void setup(const OFX::RenderArguments& args);
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "BlurProcess.tcc"

#endif
