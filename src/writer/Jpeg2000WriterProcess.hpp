#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <openjpeg/J2KWriter.hpp>


namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class Jpeg2000WriterProcess : public ImageGilFilterProcessor<View>
{
public:
    Jpeg2000WriterProcess( Jpeg2000WriterPlugin& instance );
    ~Jpeg2000WriterProcess();
	
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );

protected :
    Jpeg2000WriterPlugin&  _plugin;		///< Rendering plugin
	Jpeg2000ProcessParams  _params;
	tuttle::io::J2KWriter  _writer;		///< Writer engine

};

}
}
}
}

#include "Jpeg2000WriterProcess.tcc"

#endif
