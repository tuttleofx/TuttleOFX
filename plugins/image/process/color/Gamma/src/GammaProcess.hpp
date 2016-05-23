#ifndef _TUTTLE_PLUGIN_GAMMA_PROCESS_HPP_
#define _TUTTLE_PLUGIN_GAMMA_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace gamma
{

/**
 * @brief Gamma process
 *
 */
template <class View>
class GammaProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef float Scalar;

protected:
    GammaPlugin& _plugin; ///< Rendering plugin

public:
    GammaProcess(GammaPlugin& effect);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "GammaProcess.tcc"

#endif
