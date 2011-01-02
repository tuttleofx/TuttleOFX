#ifndef _TUTTLE_PLUGIN_CRISTO_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CRISTO_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace cristo {

/**
 * @brief Cristo process
 *
 */
template<class View>
class CristoProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    CristoPlugin&    _plugin;            ///< Rendering plugin
	CristoProcessParams<Scalar> _params; ///< parameters

public:
    CristoProcess( CristoPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "CristoProcess.tcc"

#endif
