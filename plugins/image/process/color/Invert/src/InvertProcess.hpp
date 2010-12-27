#ifndef _TUTTLE_PLUGIN_INVERTPROCESS_HPP_
#define _TUTTLE_PLUGIN_INVERTPROCESS_HPP_

#include "InvertPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

template<class View>
class InvertProcess : public ImageGilFilterProcessor<View>
{
protected:
	InvertPlugin& _plugin;  ///< Rendering plugin
	InvertProcessParams _params; ///< parameters

public:
	InvertProcess( InvertPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "InvertProcess.tcc"

#endif
