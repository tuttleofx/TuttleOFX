#ifndef _TUTTLE_PLUGIN_RAWREADERPROCESS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <libraw/libraw.h>

namespace tuttle
{
namespace plugin
{
namespace raw
{
namespace reader
{

/**
 *
 */
template <class View>
class RawReaderProcess : public ImageGilProcessor<View>
{
    typedef float Scalar;

public:
    RawReaderProcess(RawReaderPlugin& instance);

    void setup(const OFX::RenderArguments& args);
    void preProcess();
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    View& readImage(View& dst, const std::string& filepath);

private:
    // http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
    float getRedFromKelvin(const double kelvinValue);
    float getGreenFromKelvin(const double kelvinValue);
    float getBlueFromKelvin(const double kelvinValue);

private:
    RawReaderPlugin& _plugin; ///< Rendering plugin
    RawReaderProcessParams<Scalar> _params;

    LibRaw _rawProcessor;
    libraw_iparams_t& _p1;
    libraw_image_sizes_t& _size;
    libraw_colordata_t& _color;
    libraw_thumbnail_t& _thumbnail;
    libraw_imgother_t& _p2;
    libraw_output_params_t& _out;
};
}
}
}
}

#include "RawReaderProcess.tcc"

#endif
