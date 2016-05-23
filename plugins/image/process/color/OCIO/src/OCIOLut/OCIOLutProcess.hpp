#ifndef _TUTTLE_PLUGIN_OCIOLutProcess_HPP_
#define _TUTTLE_PLUGIN_OCIOLutProcess_HPP_

#include "OCIOLutPlugin.hpp"

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
namespace lut
{

namespace OCIO = OCIO_NAMESPACE;

/**
 * @brief Lut process
 */
template <class View>
class OCIOLutProcess : public ImageGilFilterProcessor<View>
{
private:
    OCIOLutPlugin& _plugin;       ///< Rendering plugin
    OCIOLutProcessParams _params; ///< parameters

    OCIO::FileTransformRcPtr fileTransform;
    OCIO::GroupTransformRcPtr groupTransform;
    OCIO::ConfigRcPtr config;

public:
    OCIOLutProcess<View>(OCIOLutPlugin& instance);
    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    // Lut Transform
    void applyLut(View& dst, View& src);
};
}
}
}
}

#include "OCIOLutProcess.tcc"

#endif
