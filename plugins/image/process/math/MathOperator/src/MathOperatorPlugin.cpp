#include "MathOperatorPlugin.hpp"
#include "MathOperatorProcess.hpp"
#include "MathOperatorDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

MathOperatorPlugin::MathOperatorPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _operator = fetchChoiceParam(kMathOperatorOperator);
    _mathOperatorType = fetchChoiceParam(kMathOperatorType);

    _master = fetchDoubleParam(kMasterValue);
    _red = fetchDoubleParam(kRedValue);
    _green = fetchDoubleParam(kGreenValue);
    _blue = fetchDoubleParam(kBlueValue);
    _alpha = fetchDoubleParam(kAlphaValue);

    _redSelect = fetchBooleanParam(kRedChannel);
    _greenSelect = fetchBooleanParam(kGreenChannel);
    _blueSelect = fetchBooleanParam(kBlueChannel);
    _alphaSelect = fetchBooleanParam(kAlphaChannel);

    updateInterface();
}

MathOperatorProcessParams<MathOperatorPlugin::Scalar>
MathOperatorPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    MathOperatorProcessParams<Scalar> params;
    EMathOperatorType type;

    params.op = static_cast<EMathOperatorMathOperator>(_operator->getValue());

    type = static_cast<EMathOperatorType>(_mathOperatorType->getValue());

    params.bRProcess = _redSelect->getValue();
    params.bGProcess = _greenSelect->getValue();
    params.bBProcess = _blueSelect->getValue();
    params.bAProcess = _alphaSelect->getValue();

    switch(type)
    {
        case eMathOperatorTypeRgba:
            switch(params.op)
            {
                case eMathOperatorOperatorPlus:
                case eMathOperatorOperatorMultiply:
                case eMathOperatorOperatorPow:
                    params.iRMathOperator = params.iGMathOperator = params.iBMathOperator = params.iAMathOperator =
                        _master->getValue();
                    break;
                case eMathOperatorOperatorSqrt:
                case eMathOperatorOperatorLog:
                case eMathOperatorOperatorLn:
                    params.iRMathOperator = params.iGMathOperator = params.iBMathOperator = params.iAMathOperator =
                        1.0; // means process channels
                    break;
            }

            break;
        case eMathOperatorTypeRgb:
            switch(params.op)
            {
                case eMathOperatorOperatorPlus:
                case eMathOperatorOperatorMultiply:
                case eMathOperatorOperatorPow:
                    params.iRMathOperator = params.iGMathOperator = params.iBMathOperator = _master->getValue();
                    params.iAMathOperator = 1.0;
                    break;
                case eMathOperatorOperatorSqrt:
                case eMathOperatorOperatorLog:
                case eMathOperatorOperatorLn:
                    params.iRMathOperator = params.iGMathOperator = params.iBMathOperator = 1.0; // means process channels
                    params.iAMathOperator = 0.0; // means not process the alpha channel
                    break;
            }

            if(params.op == eMathOperatorOperatorPlus)
                params.iAMathOperator = 0.0;
            params.bAProcess = false;

        case eMathOperatorTypeChannels:
            switch(params.op)
            {
                case eMathOperatorOperatorPlus:
                case eMathOperatorOperatorMultiply:
                case eMathOperatorOperatorPow:
                    params.iRMathOperator = _red->getValue();
                    params.iGMathOperator = _green->getValue();
                    params.iBMathOperator = _blue->getValue();
                    params.iAMathOperator = _alpha->getValue();
                    break;
                case eMathOperatorOperatorSqrt:
                case eMathOperatorOperatorLog:
                case eMathOperatorOperatorLn:
                    params.iRMathOperator = params.iGMathOperator = params.iBMathOperator = 1.0; // means process channels
                    params.iAMathOperator = 0.0; // means not process the alpha channel
                    break;
            }

            break;
    }
    return params;
}

void MathOperatorPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kMathOperatorType)
    {
        updateInterface();
    }
}

// bool MathOperatorPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	MathOperatorProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
// void MathOperatorPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter&
// rois )
//{
//	MathOperatorProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool MathOperatorPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	MathOperatorProcessParams<Scalar> params = getProcessParams();
    //	if( params._in == params._out )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MathOperatorPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<MathOperatorProcess>(*this, args);
}

void MathOperatorPlugin::updateInterface()
{
    bool bMaster = false;
    bool bRGBA = false;
    bool bAlpha = false;
    switch(getMathOperatorType())
    {
        case eMathOperatorTypeRgba:
            bMaster = false;
            bRGBA = true;
            bAlpha = false;
            break;
        case eMathOperatorTypeRgb:
            bMaster = false;
            bRGBA = true;
            bAlpha = true;
            break;
        case eMathOperatorTypeChannels:
            bMaster = true;
            bRGBA = false;
            bAlpha = false;
            break;
    }
    _master->setIsSecretAndDisabled(bMaster);
    _red->setIsSecretAndDisabled(bRGBA);
    _green->setIsSecretAndDisabled(bRGBA);
    _blue->setIsSecretAndDisabled(bRGBA);
    _alpha->setIsSecretAndDisabled(bRGBA);

    _alphaSelect->setIsSecretAndDisabled(bAlpha);
}
}
}
}
