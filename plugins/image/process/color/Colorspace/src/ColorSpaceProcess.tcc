#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/color.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorspace
{

using namespace boost::gil;

template <class View>
ColorSpaceProcess<View>::ColorSpaceProcess(ColorSpacePlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void ColorSpaceProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void ColorSpaceProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    /*
    terry::color::ColorSpaceAPI		csAPI;
    csAPI.setGammaInProperties	( _params._sGammaIn );
    csAPI.setCineonInProperties	( _params._sCineonIn );
    csAPI.setGammaOutProperties	( _params._sGammaOut );
    csAPI.setCineonOutProperties	( _params._sCineonOut );

    terry::colorspace_pixels_progress( &csAPI, _params._gradationIn, _params._layoutIn, _params._tempColorIn,
    _params._gradationOut, _params._layoutOut, _params._tempColorOut, src, dst, this );
    */
}
}
}
}
