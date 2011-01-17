#ifndef _EXR_READER_PROCESS_HPP_
#define _EXR_READER_PROCESS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <ImfInputFile.h>

#include <boost/scoped_ptr.hpp>

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
	EXRReaderPlugin&    _plugin;                        ///< Rendering plugin
	EXRReaderProcessParams _params;
	boost::scoped_ptr<Imf::InputFile>   _exrImage;      ///< Pointer to an exr image

	template<class DView>
	void channelCopy( Imf::InputFile& input, Imf::FrameBuffer& frameBuffer,
	                  DView& dst, int w, int h, int n, int left, int nc );
	template<class DView>
	void sliceCopy( const Imf::Slice* slice, DView& dst, int w, int h, int n );

public:
	EXRReaderProcess<View>( EXRReaderPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	// Read exr image
	template<class DView>
	void readImage( DView dst, const std::string& filepath );
};

}
}
}
}

#include "EXRReaderProcess.tcc"

#endif
