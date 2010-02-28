#ifndef _TUTTLE_PLUGIN_INVERTPROCESS_HPP_
#define _TUTTLE_PLUGIN_INVERTPROCESS_HPP_

#include "InvertPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

template<class View>
class InvertProcess : public ImageGilProcessor<View>
{
protected:
	InvertPlugin& _plugin;  ///< Rendering plugin
	View          _srcView; ///< Source view

public:
	InvertProcess( InvertPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "InvertProcess.tcc"

#endif
