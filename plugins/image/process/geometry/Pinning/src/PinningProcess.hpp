#ifndef _TUTTLE_PLUGIN_PINNING_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PINNING_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <terry/math/Rect.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace pinning
{

/**
 * @brief Pinning process
 *
 */
template <class View>
class PinningProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef PinningPlugin::Scalar Scalar;

protected:
    PinningPlugin& _plugin; ///< Rendering plugin

    PinningProcessParams<Scalar> _params;

public:
    PinningProcess(PinningPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

private:
    template <class Sampler>
    void resample(View& srcView, View& dstView, const terry::Rect<std::ssize_t>& procWindow,
                  const Sampler& sampler = Sampler());
};
}
}
}

#include "PinningProcess.tcc"

#endif
