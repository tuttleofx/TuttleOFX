#ifndef _TUTTLE_PLUGIN_BASICKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_BASICKEYER_PROCESS_HPP_

#include "BasicKeyerAlgorithm.hpp"

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace basicKeyer
{

/**
 * @brief BasicKeyer process
 */
template <class View>
class BasicKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef ImageGilFilterProcessor<View> Parent;
    typedef typename View::value_type Pixel;

protected:
    BasicKeyerPlugin& _plugin; ///< Rendering plugin

    BasicKeyerProcessParams<View> _params;

public:
    BasicKeyerProcess(BasicKeyerPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "BasicKeyerProcess.tcc"

#endif
