#include "OCIOColorSpaceDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <OpenColorIO/OpenColorIO.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

template <class View>
OCIOColorSpaceProcess<View>::OCIOColorSpaceProcess(OCIOColorSpacePlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void OCIOColorSpaceProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    try
    {
        // Load the current config.
        _config = _params._config;

        // Try to load the processor
        _config->getProcessor(_params._inputSpace.c_str(), _params._outputSpace.c_str());
    }
    catch(OCIO::Exception& exception)
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user(exception.what()));
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void OCIOColorSpaceProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{

    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    applyLut(dst, src);
}

template <class View>
void OCIOColorSpaceProcess<View>::applyLut(View& dst, View& src)
{
    using namespace boost::gil;
    typedef typename View::x_iterator vIterator;
    typedef typename channel_type<View>::type Pixel;

    copy_pixels(src, dst);

    try
    {
        OCIO::ConstProcessorRcPtr processor =
            _config->getProcessor(_params._inputSpace.c_str(), _params._outputSpace.c_str());

        if(is_planar<View>::value)
        {
            BOOST_THROW_EXCEPTION(exception::NotImplemented());
        }
        else
        {
            for(std::size_t y = 0; y < (unsigned int)dst.height(); ++y)
            {
                // Wrap the image in a light-weight ImageDescription
                OCIO::PackedImageDesc imageDesc((float*)&(dst(0, y)[0]), dst.width(), 1, num_channels<View>::type::value,
                                                OCIO::AutoStride, dst.pixels().pixel_size(), dst.pixels().row_size());
                // Apply the color transformation (in place)
                // Need normalized values
                processor->apply(imageDesc);
                if(this->progressForward(dst.width()))
                    return;
            }
        }
    }
    catch(OCIO::Exception& exception)
    {
        BOOST_THROW_EXCEPTION(exception::Failed() << exception::user() + "OCIO: " + exception.what());
    }
}
}
}
}
}
