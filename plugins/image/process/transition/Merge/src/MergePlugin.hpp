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

public:
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

	void render( const OFX::RenderArguments& args );

private:
	template< class View >
	void render( const OFX::RenderArguments& args );
	template< class View, template <typename> class Functor >
	void render( const OFX::RenderArguments& args );
	template< class View, template <typename> class Functor >
	void render_if( const OFX::RenderArguments& args, boost::mpl::false_ );
	template< class View, template <typename> class Functor >
	void render_if( const OFX::RenderArguments& args, boost::mpl::true_ );

public:
	OFX::Clip* _clipSrcA;               ///< Source image clip A
	OFX::Clip* _clipSrcB;               ///< Source image clip B
	OFX::Clip* _clipDst;                ///< Destination image clip

	OFX::ChoiceParam* _paramMerge;   ///< Functor structure
};

}
}
}

#endif  // MERGE_PLUGIN_H
