#ifndef _TUTTLE_PLUGIN_SEEXPR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_SEEXPR_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <SeExpression.h>

#include "SeExprAlgorithm.hpp"

namespace tuttle
{
namespace plugin
{
namespace seExpr
{

/**
 * @brief SeExpr process
 *
 */
template <class View>
class SeExprProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    SeExprPlugin& _plugin;               ///< Rendering plugin
    SeExprProcessParams<Scalar> _params; ///< parameters

public:
    SeExprProcess(SeExprPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

private:
    OfxRectD rod;
    size_t _time;
};
}
}
}

#include "SeExprProcess.tcc"

#endif
