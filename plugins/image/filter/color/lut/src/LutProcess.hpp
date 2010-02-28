#ifndef _LUT_PROCESS_HPP_
#define _LUT_PROCESS_HPP_

#include "LutPlugin.hpp"

#include "tuttle/common/utils/global.hpp"
#include "tuttle/plugin/ImageGilProcessor.hpp"
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
 * @brief Base class
 *
 */
template<class View>
class LutProcess : public ImageGilProcessor<View>
{
private:
	Lut3D *_lut3D;               ///< Lut3D
	LutPlugin&  _plugin;        ///< Rendering plugin
	View _srcView;              ///< Source view

public:
	LutProcess<View>( LutPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );

	// Lut3D Transform
	void applyLut( View& dst, View& src, const OfxRectI& procWindow );
};

}
}
}

#include "LutProcess.tcc"

#endif
