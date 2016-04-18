#ifndef _TUTTLE_PLUGIN_MATHOPERATOR_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_MATHOPERATOR_PLUGIN_HPP_

#include "MathOperatorDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

template <typename Scalar>
struct MathOperatorProcessParams
{
    EMathOperatorMathOperator op;
    double iRMathOperator;
    double iGMathOperator;
    double iBMathOperator;
    double iAMathOperator;
    bool bRProcess;
    bool bGProcess;
    bool bBProcess;
    bool bAProcess;
};

/**
 * @brief MathOperator plugin
 */
class MathOperatorPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    MathOperatorPlugin(OfxImageEffectHandle handle);

public:
    MathOperatorProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    //    OFX::Clip* _clipSrcMatte; ///< Matte source image clip

private:
    OFX::ChoiceParam* _operator;
    OFX::ChoiceParam* _mathOperatorType;

    OFX::DoubleParam* _master;
    OFX::DoubleParam* _red;
    OFX::DoubleParam* _green;
    OFX::DoubleParam* _blue;
    OFX::DoubleParam* _alpha;

    OFX::BooleanParam* _redSelect;
    OFX::BooleanParam* _greenSelect;
    OFX::BooleanParam* _blueSelect;
    OFX::BooleanParam* _alphaSelect;

    EMathOperatorType getMathOperatorType() const { return static_cast<EMathOperatorType>(_mathOperatorType->getValue()); }

    void updateInterface();
};
}
}
}

#endif
