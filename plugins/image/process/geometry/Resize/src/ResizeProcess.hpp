#ifndef _TUTTLE_PLUGIN_RESIZE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace resize {

/**
 * @brief Resize process
 *
 */
template<class View>
class ResizeProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    ResizePlugin&    _plugin;            ///< Rendering plugin
	ResizeProcessParams<Scalar> _params; ///< parameters

public:
    ResizeProcess( ResizePlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ResizeProcess.tcc"

#endif
