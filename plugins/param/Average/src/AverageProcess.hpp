#ifndef _TUTTLE_PLUGIN_AVERAGE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_AVERAGE_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>


namespace tuttle {
namespace plugin {
namespace average {

/**
 * @brief Compute the image average
 */
template<class View>
class AverageProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef boost::gil::point2<double> Point2;
    typedef typename View::value_type Pixel;
    typedef typename image_from_view<View>::type Image;
	
private:
    AveragePlugin&    _plugin;        ///< Rendering plugin
	Pixel _outputPixel;
	AverageProcessParams _processParams;
	
public:
    AverageProcess( AveragePlugin& instance );

	void setup( const OFX::RenderArguments &args );

    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "AverageProcess.tcc"

#endif
