#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

/**
 * @brief HistogramKeyer process
 *
 */
template<class View>
class HistogramKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    HistogramKeyerPlugin&    _plugin;            ///< Rendering plugin
	HistogramKeyerProcessParams<Scalar> _params; ///< parameters

public:
    HistogramKeyerProcess( HistogramKeyerPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "HistogramKeyerProcess.tcc"

#endif
