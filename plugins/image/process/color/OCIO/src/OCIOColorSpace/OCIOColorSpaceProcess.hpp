#ifndef _TUTTLE_PLUGIN_OCIOCOLORSPACEProcess_HPP_
#define _TUTTLE_PLUGIN_OCIOCOLORSPACEProcess_HPP_

#include "OCIOColorSpacePlugin.hpp"

#include <OpenColorIO/OpenColorIO.h>

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

namespace OCIO = OCIO_NAMESPACE;

/**
 * @brief Lut process
 */
template <class View>
class OCIOColorSpaceProcess : public ImageGilFilterProcessor<View>
{
private:
    OCIOColorSpacePlugin& _plugin;       ///< Rendering plugin
    OCIOColorSpaceProcessParams _params; ///< parameters

    OCIO::ConstConfigRcPtr _config;

public:
    OCIOColorSpaceProcess<View>(OCIOColorSpacePlugin& instance);
    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    // Lut Transform
    void applyLut(View& dst, View& src);
};
}
}
}
}

#include "OCIOColorSpaceProcess.tcc"

#endif
