#ifndef _TUTTLE_PLUGIN_OPERATORS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OPERATORS_PLUGIN_HPP_

#include "OperatorsDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace operators {

template<typename Scalar>
struct OperatorsProcessParams
{
	EOperatorsOperators op;	
	double              iROperators;
	double              iGOperators;
	double              iBOperators;
	double              iAOperators;
	bool                bRProcess;
	bool                bGProcess;
	bool                bBProcess;
	bool                bAProcess;
};

/**
 * @brief Operators plugin
 */
class OperatorsPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    OperatorsPlugin( OfxImageEffectHandle handle );

public:
	OperatorsProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip

private:

	OFX::ChoiceParam* _operator;
	OFX::ChoiceParam* _operatorsType;

	OFX::DoubleParam* _master;
	OFX::DoubleParam* _red;
	OFX::DoubleParam* _green;
	OFX::DoubleParam* _blue;
	OFX::DoubleParam* _alpha;

	OFX::BooleanParam* _redSelect;
	OFX::BooleanParam* _greenSelect;
	OFX::BooleanParam* _blueSelect;
	OFX::BooleanParam* _alphaSelect;

	EOperatorsType getOperatorsType() const { return static_cast<EOperatorsType>( _operatorsType->getValue() ); }
	
	void updateInterface();
	
};

}
}
}

#endif
