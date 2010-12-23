#ifndef _TUTTLE_PLUGIN_IMAGEEFFECT_GIL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IMAGEEFFECT_GIL_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {

/** 
 * @class ImageEffectGilPlugin
 * @brief 
 *
 */
class ImageEffectGilPlugin : public OFX::ImageEffect
{
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip *_clipDst; ///< Destination image clip
    OFX::Clip *_clipSrc; ///< Source image clip

public:
	ImageEffectGilPlugin( OfxImageEffectHandle handle );
	virtual ~ImageEffectGilPlugin() = 0;


	template<class Plugin, template<class> class Process, template<class Layout, class Bits>
	inline OfxStatus doGilRender( const OFX::RenderArguments &args );

	template<class Layout>
	inline OfxStatus doGilRender( const OFX::EBitDepth bitDepth, const OFX::RenderArguments &args );

	inline OfxStatus doGilRender( const OFX::EPixelComponent component, const OFX::EBitDepth bitDepth, const OFX::RenderArguments &args );

	inline OfxStatus doGilRender( const OFX::RenderArguments &args );

};

}
}

#include "ImageEffectGilPlugin.tcc"

#endif
