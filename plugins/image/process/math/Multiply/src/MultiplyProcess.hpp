#ifndef _TUTTLE_PLUGIN_MULTIPLY_PROCESS_HPP_
#define _TUTTLE_PLUGIN_MULTIPLY_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace multiply {

/**
 * @brief Multiply process
 *
 */
template<class View>
class MultiplyProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    MultiplyPlugin&    _plugin;            ///< Rendering plugin
	MultiplyProcessParams<Scalar> _params; ///< parameters

public:
    MultiplyProcess( MultiplyPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "MultiplyProcess.tcc"

#endif
