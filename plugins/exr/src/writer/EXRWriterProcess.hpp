/**
 * @file EXRWriterProcess.hpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */
#ifndef EXRWRITER_PROCESS_HPP
#define EXRWRITER_PROCESS_HPP

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ImfOutputFile.h>
#include <ImfRgba.h>
#include <ImfChannelList.h>
#include <ImfArray.h>
#include <ImathVec.h>
#include "../half/gilHalf.hpp"

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>


namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

/**
 * @brief Base class
 *
 */
template<class View>
class EXRWriterProcess : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
{
    typedef typename View::value_type value_t;
protected :
    EXRWriterPlugin&      _plugin;        ///< Rendering plugin
	OFX::StringParam*     _filepath;      ///< File path
	OFX::ChoiceParam*     _bitDepth;      ///< Bit depth
	OFX::ChoiceParam*     _componentsType;///< Components type
    View                  _srcView;       ///< Source view

	template<class Pixel>
	void writeImage( View& src, std::string& filepath, Imf::PixelType pixType ) throw( tuttle::plugin::PluginException );
public :
    EXRWriterProcess<View>(EXRWriterPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}
}

#include "EXRWriterProcess.tcc"

#endif  // EXRWRITER_PROCESS_HPP
