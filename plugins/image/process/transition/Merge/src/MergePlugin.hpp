#ifndef MERGE_PLUGIN_H
#define MERGE_PLUGIN_H

#include "gil/toolbox/MergeFunctors.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

/**
 * @brief
 *
 */
class MergePlugin : public OFX::ImageEffect
{
private:
	template<template <typename> class Functor>
	void renderGray( const OFX::RenderArguments& args );

	template<template <typename> class Functor>
	void renderRGBA( const OFX::RenderArguments& args );

public:
	MergePlugin( OfxImageEffectHandle handle );

	inline OFX::Clip* getSrcClipA() const
	{
		return _clipSrcA;
	}

	inline OFX::Clip* getSrcClipB() const
	{
		return _clipSrcB;
	}

	inline OFX::Clip* getDstClip() const
	{
		return _clipDst;
	}

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

protected:
	OFX::ChoiceParam* _mergeFunction;   ///< Functor structure
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrcA;               ///< Source image clip A
	OFX::Clip* _clipSrcB;               ///< Source image clip B
	OFX::Clip* _clipDst;                ///< Destination image clip
};

}
}
}

#endif  // MERGE_PLUGIN_H
