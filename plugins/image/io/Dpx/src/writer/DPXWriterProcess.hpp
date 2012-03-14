#ifndef _DPXWRITER_PROCESS_HPP_
#define _DPXWRITER_PROCESS_HPP_

#include <DPX.h>

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {


/**
 * @brief Dpx writer
 */
template<class View>
class DPXWriterProcess : public ImageGilFilterProcessor<View>
{
protected:
	DPXWriterPlugin&       _plugin;        ///< Rendering plugin
	DPXWriterProcessParams _params;

	void setup( const OFX::RenderArguments& args );

public:
	DPXWriterProcess( DPXWriterPlugin& instance );
};

}
}
}
}

#include "DPXWriterProcess.tcc"

#endif
