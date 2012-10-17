#ifndef _TUTTLE_PLUGIN_DUMMY_PROCESS_HPP_
#define _TUTTLE_PLUGIN_DUMMY_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace dummy {

/**
 * @brief Dummy process
 *
 */
template<class View>
class DummyProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
	DummyPlugin&    _plugin;            ///< Rendering plugin
	DummyProcessParams<Scalar> _params; ///< parameters

public:
	DummyProcess( DummyPlugin& effect );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "DummyProcess.tcc"

#endif
