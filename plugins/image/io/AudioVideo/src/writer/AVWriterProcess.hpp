#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

#include <libav/LibAVVideoWriter.hpp>

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class AVWriterProcess : public ImageGilFilterProcessor<View>
{
protected:
	AVWriterPlugin& _plugin;        ///< Rendering plugin
	AVProcessParams _params;

public:
	AVWriterProcess( AVWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "AVWriterProcess.tcc"

#endif
