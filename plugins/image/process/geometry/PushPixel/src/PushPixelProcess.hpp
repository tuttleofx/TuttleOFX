#ifndef _TUTTLE_PLUGIN_PUSHPIXEL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PUSHPIXEL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

/**
 * @brief PushPixel process
 *
 */
template <class View>
class PushPixelProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef float Scalar;
    typedef typename View::value_type Pixel;
    typedef typename terry::image_from_view<View>::type Image;
    typedef typename View::point_t Point;

protected:
    PushPixelPlugin& _plugin; ///< Rendering plugin

    PushPixelProcessParams<Scalar> _params;

    OFX::Clip* _clipMask; ///< Source image clip
    bool _clipMaskConnected;
    boost::scoped_ptr<OFX::Image> _mask;
    OfxRectI _maskPixelRod;
    View _maskView;

public:
    PushPixelProcess(PushPixelPlugin& effect);

    void preProcess()
    {
        this->progressBegin((this->_renderArgs.renderWindow.y2 - this->_renderArgs.renderWindow.y1) *
                            (this->_renderArgs.renderWindow.x2 - this->_renderArgs.renderWindow.x1) * 4);
    }

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "PushPixelProcess.tcc"

#endif
