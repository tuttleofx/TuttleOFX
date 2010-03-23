#ifndef _BITDEPTH_PROCESS_HPP_
#define _BITDEPTH_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

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
namespace bitDepth {

template<class SView, class DView>
class BitDepthProcess : public ImageGilProcessor<DView>
{
	typedef typename SView::value_type sPixel;
	typedef typename DView::value_type dPixel;
protected :
    BitDepthPlugin& _plugin;        ///< Rendering plugin
    SView               _srcView;       ///< Source view
	boost::scoped_ptr<OFX::Image> _src;
	boost::scoped_ptr<OFX::Image> _dst;

public :
    BitDepthProcess( BitDepthPlugin& instance );

    void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "BitDepthProcess.tcc"

#endif
