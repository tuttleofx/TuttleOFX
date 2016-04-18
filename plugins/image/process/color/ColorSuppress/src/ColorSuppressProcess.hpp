#ifndef _TUTTLE_PLUGIN_COLORSUPPRESS_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORSUPPRESS_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

/**
 * @brief ColorSuppress process
 *
 */
template <class View>
class ColorSuppressProcess : public ImageGilFilterProcessor<View>
{
protected:
    ColorSuppressPlugin& _plugin; ///< Rendering plugin
    ColorSuppressProcessParams _params;

public:
    ColorSuppressProcess(ColorSuppressPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorSuppressProcess.tcc"

#endif
