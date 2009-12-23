#ifndef EXR_READER_PROCESS_HPP
#define EXR_READER_PROCESS_HPP

#include <ImfInputFile.h>
#include <ImfChannelList.h>
#include <ImfArray.h>
#include <ImathVec.h>

#include "../half/gilHalf.hpp"
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
namespace exr    {
namespace reader {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class EXRReaderProcess
: public tuttle::plugin::ImageGilProcessor<View>
, public tuttle::plugin::Progress
{
protected:
	typedef typename View::value_type value_t;
	OFX::StringParam*   _filepath;                      ///< File path
	OFX::ChoiceParam*   _outComponents;                 ///< Components list
	EXRReaderPlugin&    _plugin;                        ///< Rendering plugin
	boost::scoped_ptr<Imf::InputFile>	_exrImage;		///< Pointer to an exr image

	template<class DView>
	void channelCopy(Imf::InputFile & input, Imf::FrameBuffer & frameBuffer,
					 DView & dst, int w, int h, int n, int left, int nc);
	template<class DView>
	void sliceCopy(const Imf::Slice *slice, DView & dst, int w, int h, int n);

public:
	EXRReaderProcess<View>( EXRReaderPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );

	// Read exr image
	template<class DView>
	void readImage( DView dst, std::string& filepath ) throw( tuttle::plugin::PluginException );
};

}
}
}
}

#include "EXRReaderProcess.tcc"

#endif
