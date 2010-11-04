#ifndef _TUTTLE_PLUGIN_DIFF_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DIFF_PLUGIN_HPP_

#include "DiffDefinitions.hpp"

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace quality {

struct DiffProcessParams
{};

/**
 * @brief Diff plugin
 */
class DiffPlugin : public OFX::ImageEffect
{
public:
	DiffPlugin( OfxImageEffectHandle handle );

public:
	DiffProcessParams getProcessParams() const;
	
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

	void render( const OFX::RenderArguments& args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrcA;               ///< Source image clip A
	OFX::Clip* _clipSrcB;               ///< Source image clip B
	OFX::Clip* _clipDst;                ///< Destination image clip
	OFX::RGBAParam* _qualityMesure;

};

}
}
}

#endif
