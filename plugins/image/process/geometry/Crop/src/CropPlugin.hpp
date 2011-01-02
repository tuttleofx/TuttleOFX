#ifndef _TUTTLE_PLUGIN_CROP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CROP_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

/**
 * @brief
 *
 */
class CropPlugin : public ImageEffectGilPlugin
{
public:
	CropPlugin( OfxImageEffectHandle handle );
	OfxRectD   getCropRect( const OfxRectD& rod, const double par );
	OfxRectD   getCropRect( const OfxTime time );
	bool       displayRect();

public:
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	
	void render( const OFX::RenderArguments& args );

protected:
	OFX::ChoiceParam*     _paramFormats;       ///< Image formats
	OFX::BooleanParam*    _paramOverlayRect;          ///< Display overlay rect

};

}
}
}

#endif
