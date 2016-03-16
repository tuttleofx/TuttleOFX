#ifndef _TUTTLE_PLUGIN_COLORTRANSFER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle
{
namespace plugin
{
namespace colorTransfer
{

/**
 * @brief ColorTransfer process
 *
 */
using namespace boost::numeric::ublas;

template <class View>
class ColorTransferProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef ColorTransferPlugin::Scalar Scalar;

    // Source ref image clip
    boost::scoped_ptr<OFX::Image> _srcRef;
    OfxRectI _srcRefPixelRod;
    View _srcRefView;

    // Dst ref image clip
    boost::scoped_ptr<OFX::Image> _dstRef;
    OfxRectI _dstRefPixelRod;
    View _dstRefView;

protected:
    ColorTransferPlugin& _plugin;               ///< Rendering plugin
    ColorTransferProcessParams<Scalar> _params; ///< parameters

    Pixel _srcRefAverage, _dstRefAverage, _deviationRatio;

public:
    ColorTransferProcess(ColorTransferPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

private:
    void computeAverage(const View& image, Pixel& average, Pixel& deviation, const EColorspace& eColorspace);
};
}
}
}

#include "ColorTransferProcess.tcc"

#endif
