#ifndef _TUTTLE_PLUGIN_MOVE2D_PROCESS_HPP_
#define _TUTTLE_PLUGIN_MOVE2D_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace move2D {

/**
 * @brief Move2D process
 *
 */
template<class View>
class Move2DProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    Move2DPlugin&    _plugin;            ///< Rendering plugin
	Move2DProcessParams<Scalar> _params; ///< parameters

public:
    Move2DProcess( Move2DPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "Move2DProcess.tcc"

#endif
