#include "FlipPlugin.hpp"

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace flip
{

template <class View>
FlipProcess<View>::FlipProcess(FlipPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void FlipProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time, args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void FlipProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;

    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src;
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    if(_params.flip)
    {
        /// @todo Need an option to choose the center and modify the ouput RoD.
        /// Here (inputRoD == outputRod), so we use the center of the inputRoD.
        src = subimage_view(this->_srcView, this->_dstPixelRod.x2 - procWindowOutput.x2 + this->_dstPixelRod.x1,
                            this->_dstPixelRod.y2 - procWindowOutput.y2 - this->_dstPixelRod.y1, procWindowSize.x,
                            procWindowSize.y);

        // flip_up_down_view don't modify the View type
        src = flipped_up_down_view(src);
    }
    else
    {
        src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    }

    if(_params.flop)
    {
        /// @todo Need to do the same thing than for flip.
        /// this->_dstPixelRod.x1 could be different from 0.

        // flip_left_right_view modify the View type
        copy_pixels(boost::gil::flipped_left_right_view(src), dst);
    }
    else
    {
        copy_pixels(src, dst);
    }
}
}
}
}
