#ifndef _TUTTLE_PLUGIN_HUD_PROCESS_HPP_
#define _TUTTLE_PLUGIN_HUD_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace hud {

/**
 * @brief Hud process
 *
 */
template<class View>
class HudProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    HudPlugin&    _plugin;            ///< Rendering plugin
	HudProcessParams<Scalar> _params; ///< parameters

public:
    HudProcess( HudPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "HudProcess.tcc"

#endif
