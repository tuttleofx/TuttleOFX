#ifndef _TUTTLE_PLUGIN_ADD_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_ADD_PLUGIN_HPP_

#include "AddDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace add {

template<typename Scalar>
struct AddProcessParams
{
	double iRAdd,
	       iGAdd,
	       iBAdd,
	       iAAdd;
};

/**
 * @brief Add plugin
 */
class AddPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    AddPlugin( OfxImageEffectHandle handle );

public:
	AddProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip

private:

	OFX::ChoiceParam* _addType;
	OFX::DoubleParam* _master;
	OFX::DoubleParam* _red;
	OFX::DoubleParam* _green;
	OFX::DoubleParam* _blue;
	OFX::DoubleParam* _alpha;

	EAddType getAddType() const { return static_cast<EAddType>( _addType->getValue() ); }
	
	void updateInterface();
	
};

}
}
}

#endif
