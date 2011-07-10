#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

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
namespace imageStatistics {

/**
 * @brief Compute the image average
 */
template<class View>
class ImageStatisticsProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef boost::gil::point2<double> Point2;
	typedef typename View::value_type Pixel;
	typedef typename image_from_view<View>::type Image;

private:
	ImageStatisticsPlugin&    _plugin;        ///< Rendering plugin
	Pixel _outputPixel;
	ImageStatisticsProcessParams _processParams;

public:
	ImageStatisticsProcess( ImageStatisticsPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ImageStatisticsProcess.tcc"

#endif
