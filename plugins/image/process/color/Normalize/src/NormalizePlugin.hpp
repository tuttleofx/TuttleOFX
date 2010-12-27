#ifndef _TUTTLE_PLUGIN_NORMALIZE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_PLUGIN_HPP_

#include "NormalizeDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace normalize {

template<typename Scalar>
struct NormalizeProcessParams
{
	EParamMode _mode;
	EParamAnalyseMode _analyseMode;
	
	boost::gil::rgba32f_pixel_t _srcMinColor;
	boost::gil::rgba32f_pixel_t _srcMaxColor;
	boost::gil::rgba32f_pixel_t _dstMinColor;
	boost::gil::rgba32f_pixel_t _dstMaxColor;
	bool _processR;
	bool _processG;
	bool _processB;
	bool _processA;
	
};

/**
 * @brief Normalize plugin
 */
class NormalizePlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    NormalizePlugin( OfxImageEffectHandle handle );

public:
	NormalizeProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
	OFX::ChoiceParam* _mode;
	OFX::ChoiceParam* _analyseMode;
	OFX::PushButtonParam* _analyseNow;

	OFX::GroupParam* _srcGroup;
	OFX::RGBAParam* _srcMinColor;
	OFX::RGBAParam* _srcMaxColor;
	OFX::GroupParam* _dstGroup;
	OFX::RGBAParam* _dstMinColor;
	OFX::RGBAParam* _dstMaxColor;
	OFX::GroupParam* _processGroup;
	OFX::BooleanParam* _processR;
	OFX::BooleanParam* _processG;
	OFX::BooleanParam* _processB;
	OFX::BooleanParam* _processA;
};

}
}
}

#endif
