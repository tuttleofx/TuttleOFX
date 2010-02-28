/**
 * @file BitDepthConvProcess.hpp
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */
#ifndef BITDEPTHCONV_PROCESS_HPP
#define BITDEPTHCONV_PROCESS_HPP


#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

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
namespace bitDepthConvert {

template<class SView, class DView>
class BitDepthConvProcess : public ImageGilProcessor<DView>
{
	typedef typename SView::value_type sPixel;
	typedef typename DView::value_type dPixel;
protected :
    BitDepthConvPlugin& _plugin;        ///< Rendering plugin
    SView               _srcView;       ///< Source view

public :
    BitDepthConvProcess( BitDepthConvPlugin& instance );

    void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "BitDepthConvProcess.tcc"

#endif  // BITDEPTHCONV_PROCESS_HPP
