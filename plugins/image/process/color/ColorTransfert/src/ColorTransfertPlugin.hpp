#ifndef _TUTTLE_PLUGIN_COLORTRANSFERT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFERT_PLUGIN_HPP_

#include "ColorTransfertDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

template<typename Scalar>
struct ColorTransfertProcessParams
{
	Scalar _averageCoef;
	Scalar _dynamicCoef;
};

/**
 * @brief ColorTransfert plugin
 */
class ColorTransfertPlugin : public ImageEffectGilPlugin
{
public:
	typedef double Scalar;
public:
    ColorTransfertPlugin( OfxImageEffectHandle handle );

public:
	ColorTransfertProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

	void render( const OFX::RenderArguments &args );
	
public:
        OFX::Clip* _clipSrcRef; ///< Source reference
        OFX::Clip* _clipDstRef; ///< Dest reference

        OFX::DoubleParam* _paramAverageCoef;
        OFX::DoubleParam* _paramDynamicCoef;

//        OFX::Double2DParam* _paramRegionA;
//        OFX::Double2DParam* _paramRegionB;
//        OFX::BooleanParam* _paramSameRegion;
//        OFX::Double2DParam* _paramInputRegionA;
//        OFX::Double2DParam* _paramInputRegionB;

};

}
}
}

#endif
