#include "ColorSuppressProcess.hpp"
#include "ColorSuppressPlugin.hpp"

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

template <typename T>
T luminance(const T r, const T g, const T b)
{
    static const T redCoef = 0.2989;
    static const T greenCoef = 0.5866;
    static const T blueCoef = 0.1145;

    return r * redCoef + g * greenCoef + b * blueCoef;
}

double luminance(const OfxRGBAColourD& color)
{
    return luminance(color.r, color.g, color.b);
}

template <class View>
ColorSuppressProcess<View>::ColorSuppressProcess(ColorSuppressPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void ColorSuppressProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void ColorSuppressProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    /// @todo use a gil functor
    //	transform_pixels_progress(
    //		src,
    //		dst,
    //		pixel_suppres_color_t(),
    //		*this
    //		);

    OfxRGBAColourD input, output, save;

    // For suppressing warning
    save.r = save.g = save.b = save.a = .0;

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename View::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            double pixelModified = 0;
            input.r = (*src_it)[0];
            input.g = (*src_it)[1];
            input.b = (*src_it)[2];
            input.a = (*src_it)[3];
            output = input;

            double luma1 = .0;
            if(_params.preserveLuma == true)
            {
                luma1 = luminance(input);
            }
            if(_params.output == eOutputTypeAlpha || _params.obeyAlpha == true)
            {
                save = input;
            }

            // Yellow
            if(_params.yellow != 0.0)
            {
                if(output.b < output.g && output.b < output.r)
                {
                    double diff1 = output.g - output.b;
                    double diff2 = output.r - output.b;
                    diff1 *= _params.yellow;
                    diff2 *= _params.yellow;
                    if(diff1 > diff2)
                    {
                        output.g -= diff2;
                        output.r -= diff2;
                        pixelModified += std::abs(diff2);
                    }
                    else
                    {
                        output.g -= diff1;
                        output.r -= diff1;
                        pixelModified += std::abs(diff1);
                    }
                }
            }

            // Magenta
            if(_params.magenta != 0.0)
            {
                if(output.g < output.b && output.g < output.r)
                {
                    double diff1 = output.b - output.g;
                    double diff2 = output.r - output.g;
                    diff1 *= _params.magenta;
                    diff2 *= _params.magenta;
                    if(diff1 > diff2)
                    {
                        output.b -= diff2;
                        output.r -= diff2;
                        pixelModified += std::abs(diff2);
                    }
                    else
                    {
                        output.b -= diff1;
                        output.r -= diff1;
                        pixelModified += std::abs(diff1);
                    }
                }
            }

            // Cyan
            if(_params.cyan != 0.0)
            {
                if(output.r < output.g && output.r < output.b)
                {
                    double diff1 = output.g - output.r;
                    double diff2 = output.b - output.r;
                    diff1 *= _params.cyan;
                    diff2 *= _params.cyan;
                    if(diff1 > diff2)
                    {
                        output.g -= diff2;
                        output.b -= diff2;
                        pixelModified += std::abs(diff2);
                    }
                    else
                    {
                        output.g -= diff1;
                        output.b -= diff1;
                        pixelModified += std::abs(diff1);
                    }
                }
            }

            // Red
            if(_params.red != 0.0)
            {
                if(output.r > output.g && output.r > output.b)
                {
                    double diff1 = output.r - std::max(output.g, output.b);
                    diff1 *= _params.red;
                    output.r -= diff1;
                    pixelModified += std::abs(diff1);
                }
            }

            // Green
            if(_params.green != 0.0)
            {
                if(output.g > output.b && output.g > output.r)
                {
                    double diff1 = output.g - std::max(output.b, output.r);
                    diff1 *= _params.green;
                    output.g -= diff1;
                    pixelModified += std::abs(diff1);
                }
            }

            // Blue
            if(_params.blue != 0.0)
            {
                if(output.b > output.g && output.b > output.r)
                {
                    double diff1 = output.b - std::max(output.g, output.r);
                    diff1 *= _params.blue;
                    output.b -= diff1;
                    pixelModified += std::abs(diff1);
                }
            }
            if(_params.output == eOutputTypeAlpha || _params.output == eOutputTypeAlphaImage)
            {
                output.a = pixelModified;
            }
            if(_params.preserveLuma == true && _params.output != eOutputTypeAlpha)
            {
                double luma2 = luminance(output);
                luma2 = luma1 - luma2;
                output.r += luma2;
                output.g += luma2;
                output.b += luma2;
            }
            if(_params.obeyAlpha == true)
            {
                output.r = save.a * output.r + (1.0 - save.a) * save.r;
                output.g = save.a * output.g + (1.0 - save.a) * save.g;
                output.b = save.a * output.b + (1.0 - save.a) * save.b;
                if(_params.output != eOutputTypeImage)
                {
                    output.a = save.a + (1.0 - save.a) * output.a;
                }
            }
            if(_params.output == eOutputTypeAlpha)
            {
                output.r = save.r;
                output.g = save.g;
                output.b = save.b;
            }
            (*dst_it)[0] = output.r;
            (*dst_it)[1] = output.g;
            (*dst_it)[2] = output.b;
            (*dst_it)[3] = output.a;
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
