#ifndef _TUTTLE_PLUGIN_MULTIPLY_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_MULTIPLY_PLUGIN_HPP_

#include "MultiplyDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace multiply {

template<typename Scalar>
struct MultiplyProcessParams
{
	double iRMultiply,
	       iGMultiply,
	       iBMultiply,
	       iAMultiply;
};

/**
 * @brief Multiply plugin
 */
class MultiplyPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    MultiplyPlugin( OfxImageEffectHandle handle );

public:
	MultiplyProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip

private:

	OFX::ChoiceParam* _multiplyType;
	OFX::DoubleParam* _master;
	OFX::DoubleParam* _red;
	OFX::DoubleParam* _green;
	OFX::DoubleParam* _blue;
	OFX::DoubleParam* _alpha;

	EMultiplyType getMultiplyType() const { return static_cast<EMultiplyType>( _multiplyType->getValue() ); }
	
	void updateInterface();
	
};

}
}
}

#endif
