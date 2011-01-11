#ifndef _TUTTLE_PLUGIN_MERGEPLUGIN_HPP_
#define _TUTTLE_PLUGIN_MERGEPLUGIN_HPP_

#include "MergeDefinitions.hpp"

#include <boost/gil/color_convert.hpp> // included first, to use the hack version
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

template<typename Scalar>
struct MergeProcessParams
{
	EParamRod _rod;

	boost::gil::point2<std::ptrdiff_t> _offsetA;
	boost::gil::point2<std::ptrdiff_t> _offsetB;
};

/**
 * @brief
 *
 */
class MergePlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
private:
	template<template <typename> class Functor>
	void renderGray( const OFX::RenderArguments& args );

	template<template <typename> class Functor>
	void renderRGBA( const OFX::RenderArguments& args );

public:
	MergePlugin( OfxImageEffectHandle handle );

public:
	MergeProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

//	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

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
	OFX::ChoiceParam* _paramRod;
	OFX::Int2DParam* _paramOffsetA;
	OFX::Int2DParam* _paramOffsetB;
};

}
}
}

#endif
