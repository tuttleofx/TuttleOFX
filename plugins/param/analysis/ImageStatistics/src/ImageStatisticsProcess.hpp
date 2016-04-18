#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace imageStatistics
{

/**
 * @brief Compute the image average
 */
template <class View>
class ImageStatisticsProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef boost::gil::point2<double> Point2;
    typedef typename View::value_type Pixel;
    typedef typename terry::image_from_view<View>::type Image;

private:
    ImageStatisticsPlugin& _plugin; ///< Rendering plugin
    Pixel _outputPixel;
    ImageStatisticsProcessParams _processParams;

    OFX::Clip* _clipMask; ///< Source image clip
    bool _clipMaskConnected;
    boost::scoped_ptr<OFX::Image> _mask;
    OfxRectI _maskPixelRod;
    View _maskView;

public:
    ImageStatisticsProcess(ImageStatisticsPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ImageStatisticsProcess.tcc"

#endif
