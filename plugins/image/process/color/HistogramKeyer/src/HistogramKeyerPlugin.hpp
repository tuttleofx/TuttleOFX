#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_

#include "HistogramKeyerDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

typedef std::map<double, double> Curve;

template<typename Scalar>
struct HistogramKeyerProcessParams
{
	boost::array<Curve, 6> _curves;
};

/**
 * @brief HistogramKeyer plugin
 */
class HistogramKeyerPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    HistogramKeyerPlugin( OfxImageEffectHandle handle );

public:
	HistogramKeyerProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

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
