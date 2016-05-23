#include "ConvolutionPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/memory/OfxAllocator.hpp>

namespace tuttle
{
namespace plugin
{
namespace convolution
{

template <class View>
ConvolutionProcess<View>::ConvolutionProcess(ConvolutionPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromBottomToTop)
    , _plugin(instance)
{
}

template <class View>
void ConvolutionProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ConvolutionProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    using namespace terry::filter;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    Point proc_tl(procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1);
    /*
    switch( _params._convType )
    {
            case eConv1D:
            {
            */
    if(_params._size.x == 0)
        correlate_cols_auto<Pixel>(this->_srcView, _params._convY, dst, proc_tl, _params._boundary_option);
    else if(_params._size.y == 0)
        correlate_rows_auto<Pixel>(this->_srcView, _params._convX, dst, proc_tl, _params._boundary_option);
    else
        correlate_rows_cols_auto<Pixel, OfxAllocator>(this->_srcView, _params._convX, _params._convY, dst, proc_tl,
                                                      _params._boundary_option);
    /*
            break;
    }
    case eConv2D:
    {
//			_params._convMatrix
            break;
    }
}
*/
}
}
}
}
