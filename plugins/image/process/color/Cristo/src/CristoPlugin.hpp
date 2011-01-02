#ifndef _TUTTLE_PLUGIN_CRISTO_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CRISTO_PLUGIN_HPP_

#include "CristoDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace cristo {

typedef std::map<double, double> Curve;

template<typename Scalar>
struct CristoProcessParams
{
	boost::array<Curve, 6> _curves;
};

/**
 * @brief Cristo plugin
 */
class CristoPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    CristoPlugin( OfxImageEffectHandle handle );

public:
	CristoProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip
	OFX::ParametricParam* _paramColorSelection;
};

}
}
}

#endif
