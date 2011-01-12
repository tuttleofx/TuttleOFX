#ifndef _TUTTLE_PLUGIN_FLOODFILL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {

/**
 * @brief FloodFill process
 *
 */
template<class View>
class FloodFillProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected :
    FloodFillPlugin&    _plugin;            ///< Rendering plugin
	FloodFillProcessParams<Scalar> _params; ///< parameters

	bool _isConstantImage;
	Scalar _lowerThres;
	Scalar _upperThres;

public:
    FloodFillProcess( FloodFillPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "FloodFillProcess.tcc"

#endif
