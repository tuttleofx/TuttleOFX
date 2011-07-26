#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

/**
 * @brief ColorSpaceKeyer process
 *
 */
template<class View>
class ColorSpaceKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    ColorSpaceKeyerPlugin&    _plugin;            ///< Rendering plugin
	ColorSpaceKeyerProcessParams<Scalar> _params; ///< parameters

public:
    ColorSpaceKeyerProcess( ColorSpaceKeyerPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorSpaceKeyerProcess.tcc"

#endif
