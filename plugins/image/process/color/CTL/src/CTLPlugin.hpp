#ifndef _TUTTLE_PLUGIN_CTL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CTL_PLUGIN_HPP_

#include "CTLDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace ctl {

template<typename Scalar>
struct CTLProcessParams
{
	std::string _code;
};

/**
 * @brief CTL plugin
 */
class CTLPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    CTLPlugin( OfxImageEffectHandle handle );

public:
	CTLProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
	OFX::ChoiceParam* _paramInput;
	OFX::StringParam* _paramCode;
	OFX::StringParam* _paramFile;
	
private:
	OFX::InstanceChangedArgs _instanceChangedArgs;
};

}
}
}

#endif
