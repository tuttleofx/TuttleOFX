#ifndef _BITDEPTH_PROCESS_HPP_
#define _BITDEPTH_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace bitDepth
{

template <class SView, class DView>
class BitDepthProcess : public ImageGilProcessor<DView>
{
    typedef typename SView::value_type sPixel;
    typedef typename DView::value_type dPixel;

protected:
    BitDepthPlugin& _plugin; ///< Rendering plugin
    OFX::Clip* _clipSrc;     ///< Source image clip
    boost::scoped_ptr<OFX::Image> _src;
    OfxRectI _srcPixelRod;
    SView _srcView; ///< @brief source clip (filters have only one input)

public:
    BitDepthProcess(BitDepthPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "BitDepthProcess.tcc"

#endif
