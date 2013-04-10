#ifndef _TUTTLE_PLUGIN_SWSCALE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_SWSCALE_PROCESS_HPP_

#include "SwscalePlugin.hpp"

#include <tuttle/plugin/ImageFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace swscale {

/**
 * @brief Swscale process
 *
 */
class SwscaleProcess : public ImageFilterProcessor
{
protected:
	SwscalePlugin&       _plugin; ///< Rendering plugin
	SwscaleProcessParams _params; ///< parameters
	struct SwsContext*   _context;

public:
	SwscaleProcess( SwscalePlugin& effect );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
	
	void postProcess();
};

}
}
}

#endif
