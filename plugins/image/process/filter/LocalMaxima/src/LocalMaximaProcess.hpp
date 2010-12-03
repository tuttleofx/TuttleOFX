#ifndef _TUTTLE_PLUGIN_LOCALMAXIMA_PROCESS_HPP_
#define _TUTTLE_PLUGIN_LOCALMAXIMA_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace localmaxima {

/**
 * @brief LocalMaxima process
 *
 */
template<class View>
class LocalMaximaProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected :
    LocalMaximaPlugin&    _plugin;            ///< Rendering plugin
	LocalMaximaProcessParams<Scalar> _params; ///< parameters

public:
    LocalMaximaProcess( LocalMaximaPlugin& effect );

	void setup( const OFX::RenderArguments& args );
	
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "LocalMaximaProcess.tcc"

#endif
