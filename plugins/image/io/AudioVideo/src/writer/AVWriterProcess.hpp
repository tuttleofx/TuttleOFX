#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

#include "AVWriterPlugin.hpp"

#include <AvTranscoder/essenceStructures/VideoFrame.hpp>

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <terry/globals.hpp>

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
	AVWriterPlugin& _plugin; ///< Rendering plugin
	AVProcessParams _params;

public:
	AVWriterProcess( AVWriterPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "AVWriterProcess.tcc"

#endif
