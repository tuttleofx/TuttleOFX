#ifndef _TUTTLE_PLUGIN_LOCALMAXIMA_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_LOCALMAXIMA_PLUGIN_HPP_

#include "LocalMaximaDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>


namespace tuttle {
namespace plugin {
namespace localmaxima {

template<typename Scalar>
struct LocalMaximaProcessParams
{
	EParamBorder _border;
};

/**
 * @brief LocalMaxima plugin
 */
class LocalMaximaPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
	LocalMaximaPlugin( OfxImageEffectHandle handle );

public:
	LocalMaximaProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

	void render( const OFX::RenderArguments &args );
	
public:
	OFX::ChoiceParam* _paramBorder;
	OFX::ChoiceParam* _paramOutputComponent;
};

}
}
}

#endif
