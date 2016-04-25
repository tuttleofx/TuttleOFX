#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include "GammaPlugin.hpp"

namespace tuttle
{
namespace plugin
{
namespace gamma
{

template <class View>
GammaProcess<View>::GammaProcess(GammaPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void GammaProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    GammaProcessParams<GammaPlugin::Scalar> params = _plugin.getProcessParams();
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    rgba32f_pixel_t wpix;

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename View::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            // x^a = e^aln(x)
            color_convert(*src_it, wpix);
            if(wpix[0] > 0.0)
            {
                wpix[0] = exp(log(wpix[0]) * params.iRGamma);
            }

            if(wpix[1] > 0.0)
            {
                wpix[1] = exp(log(wpix[1]) * params.iGGamma);
            }

            if(wpix[2] > 0.0)
            {
                wpix[2] = exp(log(wpix[2]) * params.iBGamma);
            }

            if(wpix[3] > 0.0)
            {
                wpix[3] = exp(log(wpix[3]) * params.iAGamma);
            }
            color_convert(wpix, *dst_it);
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
    /*
       const OfxRectI procWindowSrc = this->translateRegion( procWindowRoW, this->_srcPixelRod );
       OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
                                 procWindowRoW.y2 - procWindowRoW.y1 };
       View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
                              procWindowSize.x,
                              procWindowSize.y );
       View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
                              procWindowSize.x,
                              procWindowSize.y );
       copy_pixels( src, dst );
     */
}
}
}
}
