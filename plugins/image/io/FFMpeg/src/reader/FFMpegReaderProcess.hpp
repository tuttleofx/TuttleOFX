#ifndef _TUTTLE_PLUGIN_FFMPEG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_READER_PROCESS_HPP_

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class FFMpegReaderProcess : public ImageGilProcessor<View>
{
protected:
	FFMpegReaderPlugin& _plugin;        ///< Rendering plugin

public:
	FFMpegReaderProcess( FFMpegReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	// Do some processing
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "FFMpegReaderProcess.tcc"

#endif
