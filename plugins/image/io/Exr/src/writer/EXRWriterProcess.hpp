#ifndef _EXRWRITER_PROCESS_HPP_
#define _EXRWRITER_PROCESS_HPP_

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ImfOutputFile.h>
#include <ImfRgba.h>
#include <ImfChannelList.h>
#include <ImfArray.h>
#include <ImathVec.h>
#include "../half/gilHalf.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

/**
 * @brief Exr writer
 */
template<class View>
class EXRWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

public:
	EXRWriterProcess( EXRWriterPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

protected:
	EXRWriterPlugin&      _plugin;        ///< Rendering plugin
	EXRWriterProcessParams _params;

	template<class WPixel>
	void writeImage( View& src, std::string& filepath, Imf::PixelType pixType );

};

}
}
}
}

#include "EXRWriterProcess.tcc"

#endif
