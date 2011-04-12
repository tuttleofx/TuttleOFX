#ifndef _TUTTLE_PLUGIN_PRINT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PRINT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace print {

/**
 * @brief Print process
 *
 */
template<class View>
class PrintProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    PrintPlugin&    _plugin;            ///< Rendering plugin
	PrintProcessParams<Scalar> _params; ///< parameters

public:
    PrintProcess( PrintPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "PrintProcess.tcc"

#endif
