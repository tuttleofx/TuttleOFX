#ifndef _TUTTLE_PLUGIN_DIFF_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DIFF_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace quality {

struct DiffProcessParams
{
	
};

/**
 * @brief Diff plugin
 */
class DiffPlugin : public OFX::ImageEffect
{
public:
    DiffPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClipA( ) const;
    OFX::Clip *getSrcClipB( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

	DiffProcessParams getProcessParams() const;

public:
    // do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClipA;               ///< Source image clip A
	OFX::Clip* _srcClipB;               ///< Source image clip B
    OFX::Clip* _dstClip;                ///< Destination image clip
	OFX::RGBAParam* _qualityMesure;

};

}
}
}

#endif
