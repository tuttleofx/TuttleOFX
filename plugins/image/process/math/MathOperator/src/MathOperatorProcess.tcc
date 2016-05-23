
#include "MathOperatorPlugin.hpp"
#include "MathOperatorAlgorithm.hpp"

#include <cmath>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

template <class View>
MathOperatorProcess<View>::MathOperatorProcess(MathOperatorPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void MathOperatorProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void MathOperatorProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput;

    MathOperatorProcessParams<MathOperatorPlugin::Scalar> params = _plugin.getProcessParams();
    procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);

    rgba32f_pixel_t valuesPix;
    valuesPix[0] = params.iRMathOperator;
    valuesPix[1] = params.iGMathOperator;
    valuesPix[2] = params.iBMathOperator;
    valuesPix[3] = params.iAMathOperator;

    rgba8_pixel_t processChannel;
    processChannel[0] = params.bRProcess;
    processChannel[1] = params.bGProcess;
    processChannel[2] = params.bBProcess;
    processChannel[3] = params.bAProcess;

    switch(params.op)
    {
        case eMathOperatorOperatorPlus:
            processImagePlus(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
        case eMathOperatorOperatorMultiply:
            processImageMultiply(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
        case eMathOperatorOperatorPow:
            processImagePow(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
        case eMathOperatorOperatorSqrt:
            processImageSqrt(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
        case eMathOperatorOperatorLog:
            processImageLog(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
        case eMathOperatorOperatorLn:
            processImageLn(procWindowOutput, procWindowRoW, valuesPix, processChannel);
            break;
    }
}
}
}
}
