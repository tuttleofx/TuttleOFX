#ifndef _TUTTLE_PLUGIN_DIFF_PROCESS_HPP_
#define _TUTTLE_PLUGIN_DIFF_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace quality
{

/**
 * @brief Diff process
 *
 */
template <class View>
class DiffProcess : public ImageGilProcessor<View>
{
    typedef typename boost::gil::pixel<boost::gil::bits32f,
                                       boost::gil::layout<typename boost::gil::color_space_type<View>::type> > PixelF;

protected:
    DiffPlugin& _plugin;       ///< Rendering plugin
    DiffProcessParams _params; ///< Rendering parameters
    View _srcViewA;            ///< Source view A
    View _srcViewB;            ///< Source view B
    boost::scoped_ptr<OFX::Image> _srcA;
    boost::scoped_ptr<OFX::Image> _srcB;
    OfxRectI _srcPixelRodA;
    OfxRectI _srcPixelRodB;

public:
    DiffProcess(DiffPlugin& instance);
    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
    template <class SView>
    boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
    mse(const SView& v1, const SView& v2, const SView& dst, bool outputIsPsnr = false);
    template <class SView>
    boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
    psnr(const SView& v1, const SView& v2, const SView& dst);
};
}
}
}

#include "DiffProcess.tcc"

#endif
