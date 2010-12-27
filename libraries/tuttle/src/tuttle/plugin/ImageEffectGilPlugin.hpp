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

template<template<class> class Process, class Plugin, class Layout, class Bits>
inline void doGilRender( Plugin& plugin, const OFX::RenderArguments &args );

template<template<class> class Process, class Plugin, class Layout>
inline void doGilRender( Plugin& plugin, const OFX::RenderArguments &args, const OFX::EBitDepth bitDepth );

template<template<class> class Process, class Plugin>
inline void doGilRender( Plugin& plugin, const OFX::RenderArguments &args, const OFX::EPixelComponent component, const OFX::EBitDepth bitDepth );

template<template<class> class Process, class Plugin>
inline void doGilRender( Plugin& plugin, const OFX::RenderArguments &args );


}
}

#include "ImageEffectGilPlugin.tcc"

#endif
