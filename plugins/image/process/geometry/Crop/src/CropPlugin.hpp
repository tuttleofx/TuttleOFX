#ifndef VOLET_PLUGIN_H
#define VOLET_PLUGIN_H

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
	void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

protected:
	OFX::ChoiceParam*     _formats;       ///< Image formats
	OFX::BooleanParam*    _rect;          ///< Display overlay rect

};

}
}
}

#endif
