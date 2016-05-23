#ifndef _TUTTLE_PLUGIN_MATHOPERATOR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_MATHOPERATOR_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

/**
 * @brief MathOperator process
 *
 */
template <class View>
class MathOperatorProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    MathOperatorPlugin& _plugin;               ///< Rendering plugin
    MathOperatorProcessParams<Scalar> _params; ///< parameters

public:
    MathOperatorProcess(MathOperatorPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

private:
    void processImagePlus(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW,
                          const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel);
    void processImageMultiply(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW,
                              const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel);
    void processImagePow(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW,
                         const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel);
    void processImageSqrt(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW,
                          const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel);
    void processImageLog(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW,
                         const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel);
    void processImageLn(OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values,
                        const boost::gil::rgba8_pixel_t& processChannel);
};
}
}
}

#include "MathOperatorProcess.tcc"

#endif
