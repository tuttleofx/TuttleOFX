#ifndef _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

#include "ColorDistributionDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace colorDistribution {

template<typename Scalar>
struct ColorDistributionProcessParams
{
	bool _invert;
	EParamDistribution _in;
	EParamDistribution _out;
};

/**
 * @brief ColorDistribution plugin
 */
class ColorDistributionPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    ColorDistributionPlugin( OfxImageEffectHandle handle );

public:
	ColorDistributionProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip*			_clipSrc; ///< Source image clip
    OFX::Clip*			_clipDst; ///< Destination image clip
	OFX::ChoiceParam	*_paramIn;
	OFX::ChoiceParam	*_paramOut;
};

}
}
}

#endif
