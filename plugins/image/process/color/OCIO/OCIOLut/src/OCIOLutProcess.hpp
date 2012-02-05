#ifndef _TUTTLE_PLUGIN_OCIOLutProcess_HPP_
#define _TUTTLE_PLUGIN_OCIOLutProcess_HPP_

#include "OCIOLutPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace ocio{
namespace lut {

/**
 * @brief Lut process
 */
template<class View>
class OCIOLutProcess : public ImageGilFilterProcessor<View>
{
private:
	OCIOLutPlugin&  _plugin;        ///< Rendering plugin

public:
	OCIOLutProcess<View>( OCIOLutPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	// Lut Transform
	void applyLut( View& dst, View& src );
};

}
}
}
}

#include "OCIOLutProcess.tcc"

#endif
