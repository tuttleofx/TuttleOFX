#ifndef _TUTTLE_PLUGIN_THINNING_PROCESS_HPP_
#define _TUTTLE_PLUGIN_THINNING_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

/**
 * @brief Thinning process
 *
 */
template<class View>
class ThinningProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected :
    ThinningPlugin&    _plugin;            ///< Rendering plugin
	ThinningProcessParams<Scalar> _params; ///< parameters

public:
    ThinningProcess( ThinningPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ThinningProcess.tcc"

#endif
