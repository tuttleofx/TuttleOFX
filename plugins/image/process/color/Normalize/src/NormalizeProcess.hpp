#ifndef _TUTTLE_PLUGIN_NORMALIZE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/gil/channel_algorithm.hpp>

namespace tuttle
{
namespace plugin
{
namespace normalize
{

/**
 * @brief Normalize process
 *
 */
template <class View>
class NormalizeProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;

    typedef typename boost::gil::channel_type<View>::type d_channel_t;
    // typedef typename boost::gil::channel_convert_to_unsigned<d_channel_t>::type channel_t;

    typedef boost::gil::pixel<boost::gil::bits32f, d_channel_t> Ratio_pixel_t;

    typedef float Scalar;

protected:
    NormalizePlugin& _plugin;               ///< Rendering plugin
    NormalizeProcessParams<Scalar> _params; ///< parameters

    /// @brief Processing datas
    /// @{
    boost::gil::rgba32f_pixel_t _sMin;  ///< min value in source image
    boost::gil::rgba32f_pixel_t _dMin;  ///< min value in destination image
    boost::gil::rgba32f_pixel_t _ratio; ///< scale to go from source to dest
    /// @}
    /*
            template< class WorkView, typename LocalChannel >
            void processChannel( View& src, View& dst, bool process, Pixel& ratio, Pixel& sMin, Pixel& dMin );*/

    template <class WorkView, typename LocalChannel, typename P>
    void processChannel(View& src, View& dst, bool process, P& ratio, P& sMin, P& dMin);

public:
    NormalizeProcess(NormalizePlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "NormalizeProcess.tcc"

#endif
