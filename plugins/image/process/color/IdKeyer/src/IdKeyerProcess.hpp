#ifndef _TUTTLE_PLUGIN_IDKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IDKEYER_PROCESS_HPP_

#include "IdKeyerAlgorithm.hpp"

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
namespace idKeyer
{

/**
 * @brief IdKeyer process
 */
template <class View>
class IdKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef ImageGilFilterProcessor<View> Parent;
    typedef typename View::value_type Pixel;

public:
    IdKeyerProcess(IdKeyerPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

protected:
    IdKeyerPlugin& _plugin;
    IdKeyerProcessParams<View> _params;
};
}
}
}

#include "IdKeyerProcess.tcc"

#endif
