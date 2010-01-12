/**
 * @file LutProcess.hpp
 * @brief
 * @author
 * @date    01/10/09 12:01
 *
 */
#ifndef LUT_PROCESS_HPP
#define LUT_PROCESS_HPP

#include "LutPlugin.hpp"

#include "tuttle/common/utils/global.hpp"
#include "tuttle/plugin/ImageGilProcessor.hpp"
#include "tuttle/plugin/Progress.hpp"
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
class LutProcess
	: public tuttle::plugin::ImageGilProcessor<View>,
	public tuttle::plugin::Progress
{
typedef typename View::value_type value_t;

private:
	Lut3D _lut3D;               ///< Lut3D
	LutPlugin&  _plugin;        ///< Rendering plugin
	View _srcView;              ///< Source view

public:
	LutProcess<View>( LutPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );

	// Lut3D Transform
	void applyLut( View& dst, View& src );
};

}
}
}

#include "LutProcess.tcc"

#endif  // LUT_PROCESS_HPP
