#ifndef _TUTTLE_PLUGIN_FLOODFILL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_PLUGIN_HPP_

#include "FloodFillDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {

template<typename Scalar>
struct FloodFillProcessParams
{
	Scalar _upperThres;
	Scalar _lowerThres;
	bool _relativeMinMax;
	EParamMethod _method;
};

/**
 * @brief FloodFill plugin
 */
class FloodFillPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    FloodFillPlugin( OfxImageEffectHandle handle );

public:
	FloodFillProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

    void render( const OFX::RenderArguments &args );
	
	
public:
    OFX::DoubleParam* _paramUpperThres;
    OFX::DoubleParam* _paramLowerThres;
    OFX::BooleanParam* _paramRelativeMinMax;
    OFX::ChoiceParam* _paramMethod;
};

}
}
}

#endif
