#ifndef _TUTTLE_PLUGIN_FLIP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FLIP_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include "FlipDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace flip {

struct FlipProcessParams
{
        bool flip;
        bool flop;
};

/**
 * @brief
 *
 */
class FlipPlugin : public ImageEffectGilPlugin
{
public:
        FlipPlugin( OfxImageEffectHandle handle );

public:
        FlipProcessParams getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

        OfxRectI getFlipRegionValue() const;
        OfxRectI computeFlipRegion( const OfxTime time, const bool fromRatio = false ) const;
	
	void render( const OFX::RenderArguments& args );

        void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

private:
        OFX::Clip* _clipSrcRef; ///< Source reference
        OFX::Clip* _clipDstRef; ///< Dest reference

        OFX::BooleanParam* _paramFlip;
        OFX::BooleanParam* _paramFlop;
};

}
}
}

#endif
