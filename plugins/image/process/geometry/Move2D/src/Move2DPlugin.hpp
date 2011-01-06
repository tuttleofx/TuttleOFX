#ifndef _TUTTLE_PLUGIN_MOVE2D_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_MOVE2D_PLUGIN_HPP_

#include "Move2DDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace move2D {

template<typename Scalar>
struct Move2DProcessParams
{
	boost::gil::point2<Scalar> _translation;
};

/**
 * @brief Move2D plugin
 */
class Move2DPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    Move2DPlugin( OfxImageEffectHandle handle );

public:
	Move2DProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip
	OFX::Double2DParam* _paramTranslation;
};

}
}
}

#endif
