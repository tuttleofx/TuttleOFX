#ifndef _TUTTLE_PLUGIN_FADE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FADE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace fade
{

/**
 * @brief Fade process
 *
 */
template <class View>
class FadeProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    FadePlugin& _plugin;       ///< Rendering plugin
    FadeProcessParams _params; ///< parameters

    View _srcViewA; ///< Source view A
    View _srcViewB; ///< Source view B
    boost::scoped_ptr<OFX::Image> _srcA;
    boost::scoped_ptr<OFX::Image> _srcB;
    OfxRectI _srcPixelRodA;
    OfxRectI _srcPixelRodB;

public:
    FadeProcess(FadePlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "FadeProcess.tcc"

#endif
