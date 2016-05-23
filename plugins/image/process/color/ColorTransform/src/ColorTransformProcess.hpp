#ifndef _TUTTLE_PLUGIN_COLORTRANSFORM_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFORM_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransform
{

/**
 * @brief ColorTransform process
 *
 */
template <class View>
class ColorTransformProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    ColorTransformPlugin& _plugin; ///< Rendering plugin
    ColorTransformParams _params;  ///< parameters

    typedef terry::math::BoundedMatrix<Scalar, 5, 5>::Type BoundedMatrix5x5;
    BoundedMatrix5x5 _matrix;

public:
    ColorTransformProcess(ColorTransformPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorTransformProcess.tcc"

#endif
