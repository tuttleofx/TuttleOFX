#ifndef _TUTTLE_PLUGIN_ADD_PROCESS_HPP_
#define _TUTTLE_PLUGIN_ADD_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace add {

/**
 * @brief Add process
 *
 */
template<class View>
class AddProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    AddPlugin&    _plugin;            ///< Rendering plugin
	AddProcessParams<Scalar> _params; ///< parameters

public:
    AddProcess( AddPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "AddProcess.tcc"

#endif
