#ifndef _LUT_PROCESS_HPP_
#define _LUT_PROCESS_HPP_

#include "LutPlugin.hpp"

#include "tuttle/common/utils/global.hpp"
#include "tuttle/plugin/ImageGilFilterProcessor.hpp"
#include "tuttle/plugin/PluginException.hpp"
#include "lutEngine/lut_reader.h"
#include "lutEngine/hd3d_lut.h"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace lut {

/**
 * @brief Lut process
 */
template<class View>
class LutProcess : public ImageGilFilterProcessor<View>
{
private:
	Lut3D *_lut3D;               ///< Lut3D
	LutPlugin&  _plugin;        ///< Rendering plugin

public:
	LutProcess<View>( LutPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	// Lut3D Transform
	void applyLut( View& dst, View& src, const OfxRectI& procWindow );
};

}
}
}

#include "LutProcess.tcc"

#endif
