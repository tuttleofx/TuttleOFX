#ifndef _TUTTLE_PLUGIN_NORMALIZE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_PLUGIN_HPP_

#include "NormalizeDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

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
class NormalizePlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    NormalizePlugin( OfxImageEffectHandle handle );

public:
	NormalizeProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

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
