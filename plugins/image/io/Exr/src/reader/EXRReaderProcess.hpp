#ifndef _EXR_READER_PROCESS_HPP_
#define _EXR_READER_PROCESS_HPP_

#include <ImfInputFile.h>
#include <ImfChannelList.h>
#include <ImfArray.h>
#include <ImathVec.h>

#include "../half/gilHalf.hpp"
#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
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
 * @brief Exr reader
 */
template<class View>
class EXRReaderProcess : public ImageGilProcessor<View>
{
protected:
	typedef typename View::value_type Pixel;
	OFX::StringParam*   _filepath;                      ///< File path
	OFX::ChoiceParam*   _outComponents;                 ///< Components list
	EXRReaderPlugin&    _plugin;                        ///< Rendering plugin
	boost::scoped_ptr<Imf::InputFile>   _exrImage;      ///< Pointer to an exr image

	template<class DView>
	void channelCopy( Imf::InputFile& input, Imf::FrameBuffer& frameBuffer,
	                  DView& dst, int w, int h, int n, int left, int nc );
	template<class DView>
	void sliceCopy( const Imf::Slice* slice, DView& dst, int w, int h, int n );

public:
	EXRReaderProcess<View>( EXRReaderPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );

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
