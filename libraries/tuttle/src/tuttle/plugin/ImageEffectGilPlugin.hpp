#ifndef _TUTTLE_PLUGIN_IMAGEEFFECT_GIL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IMAGEEFFECT_GIL_PLUGIN_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {

/** 
 * @brief 
 *
 */
class ImageEffectGilPlugin : public OFX::ImageEffect
{
public:
	ImageEffectGilPlugin( OfxImageEffectHandle handle );
	virtual ~ImageEffectGilPlugin() = 0;

public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip *_clipDst; ///< Destination image clip
    OFX::Clip *_clipSrc; ///< Source image clip
};


template< template<class> class Process,
          bool planar, class Layout, class Bits,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args );

template< template<class> class Process,
	      bool planar, class Layout,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth bitDepth );

template< template<class> class Process,
	      bool planar,
          class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent components, const OFX::EBitDepth bitDepth );

template< template<class> class Process,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::Clip& clip );

/**
 * @brief This render function, instanciate a Process class templated with the image type (layout and bit depth).
 * @param[in]   args     Rendering parameters
 */
template< template<class> class Process,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args );

template< template<class,class> class Process,
          bool splanar, class SLayout, class SBits,
		  bool dplanar, class DLayout, class DBits,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args );

template< template<class,class,class> class Process,
          bool s1planar, class S1Layout, class S1Bits,
          bool s2planar, class S2Layout, class S2Bits,
		  bool dplanar, class DLayout, class DBits,
          class Plugin>
void doGilRender3( Plugin& plugin, const OFX::RenderArguments& args );

}
}

#include "ImageEffectGilPlugin.tcc"

#endif
