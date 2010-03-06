#ifndef _BITDEPTHCONV_PROCESS_HPP_
#define _BITDEPTHCONV_PROCESS_HPP_

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
namespace bitDepthConvert {

template<class SView, class DView>
class BitDepthConvProcess : public ImageGilProcessor<DView>
{
	typedef typename SView::value_type sPixel;
	typedef typename DView::value_type dPixel;
protected :
    BitDepthConvPlugin& _plugin;        ///< Rendering plugin
    SView               _srcView;       ///< Source view
	boost::scoped_ptr<OFX::Image> _src;
	boost::scoped_ptr<OFX::Image> _dst;

public :
    BitDepthConvProcess( BitDepthConvPlugin& instance );

    void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "BitDepthConvProcess.tcc"

#endif
