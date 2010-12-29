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
template<class SView, class DView>
class LocalMaximaProcess : public ImageGilFilterProcessor<SView, DView>
{
public:
	typedef typename SView::value_type SPixel;
	typedef typename boost::gil::channel_type<SView>::type SChannel;

	typedef typename DView::value_type DPixel;
	typedef typename boost::gil::channel_type<DView>::type DChannel;

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
