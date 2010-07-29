#ifndef _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

template<typename Scalar>
struct ColorDistributionProcessParams
{
	bool invert;
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
    void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	
	ColorDistributionProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip*			_srcClip; ///< Source image clip
    OFX::Clip*			_dstClip; ///< Destination image clip
	OFX::BooleanParam	*_invert;
};

}
}
}

#endif
