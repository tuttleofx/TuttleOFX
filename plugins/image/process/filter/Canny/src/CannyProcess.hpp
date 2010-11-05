#ifndef _TUTTLE_PLUGIN_CANNY_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CANNY_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace canny {

/**
 * @brief Canny process
 *
 */
template<class View>
class CannyProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected :
    CannyPlugin&    _plugin;            ///< Rendering plugin
	CannyProcessParams<Scalar> _params; ///< parameters

public:
    CannyProcess( CannyPlugin& effect );

	void setup( const OFX::RenderArguments& args );
	
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "CannyProcess.tcc"

#endif
