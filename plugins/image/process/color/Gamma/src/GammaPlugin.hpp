#ifndef _TUTTLE_PLUGIN_GAMMA_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_GAMMA_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

#include "GammaDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace gamma {

template<typename Scalar>
struct GammaProcessParams
{
	double iRGamma,
	       iGGamma,
	       iBGamma,
	       iAGamma;
};

/**
 * @brief Gamma plugin
 */
class GammaPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;

public:
	GammaPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	GammaProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc; ///< Source image clip
	OFX::Clip* _clipDst; ///< Destination image clip
	OFX::ChoiceParam* _gammaType;
	OFX::DoubleParam* _master;
	OFX::DoubleParam* _red;
	OFX::DoubleParam* _green;
	OFX::DoubleParam* _blue;
	OFX::DoubleParam* _alpha;
	OFX::BooleanParam* _invert;
	EGammaType getGammaType() const { return static_cast<EGammaType>( _gammaType->getValue() ); }
};

}
}
}

#endif
