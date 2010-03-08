#ifndef _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace debugImageEffectApi {

struct DebugImageEffectApiParams
{
	
};

/**
 * @brief DebugImageEffectApi plugin
 */
class DebugImageEffectApiPlugin : public OFX::ImageEffect
{
public:
    DebugImageEffectApiPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	
	DebugImageEffectApiParams getParams() const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_srcClip;       ///< Source image clip
    OFX::Clip           *_dstClip;       ///< Destination image clip
	
	OFX::StringParam* _hostInfos;
	OFX::DoubleParam* _currentTime;
	OFX::DoubleParam* _beginTime;
	OFX::DoubleParam* _endTime;
};

}
}
}

#endif
