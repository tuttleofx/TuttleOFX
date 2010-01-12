#ifndef PNG_READER_PROCESS_HPP
#define PNG_READER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class PNGReaderProcess : public tuttle::plugin::ImageGilProcessor<View>,
	public tuttle::plugin::Progress
{
typedef typename View::value_type value_t;

protected:
	OFX::StringParam*   _filepath;      ///< File path
	PNGReaderPlugin&    _plugin;        ///< Rendering plugin

public:
	PNGReaderProcess<View>( PNGReaderPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );

	//
	View& readImage( View& dst, std::string& filepath ) throw( PluginException );
};

}
}
}
}

#include "PNGReaderProcess.tcc"

#endif
