#ifndef _TUTTLE_COLORSPACE_PROCESS_HPP_
#define _TUTTLE_COLORSPACE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorspace
{

template <class View>
class ColorSpaceProcess : public ImageGilFilterProcessor<View>
{
protected:
    ColorSpaceProcessParams _params;

    ColorSpacePlugin& _plugin; ///< Rendering plugin

public:
    ColorSpaceProcess(ColorSpacePlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorSpaceProcess.tcc"

#endif
