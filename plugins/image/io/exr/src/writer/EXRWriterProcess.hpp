#ifndef _EXRWRITER_PROCESS_HPP_
#define _EXRWRITER_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

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
protected:
	EXRWriterPlugin&      _plugin;        ///< Rendering plugin
	OFX::StringParam*     _filepath;      ///< File path
	OFX::ChoiceParam*     _bitDepth;      ///< Bit depth
	OFX::ChoiceParam*     _componentsType; ///< Components type

	template<class Pixel>
	void writeImage( View& src, std::string& filepath, Imf::PixelType pixType ) throw( tuttle::plugin::PluginException );

public:
	EXRWriterProcess( EXRWriterPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "EXRWriterProcess.tcc"

#endif
