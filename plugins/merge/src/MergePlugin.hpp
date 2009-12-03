/**
 * @file MergePlugin.hpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */

#ifndef MERGE_PLUGIN_H
#define MERGE_PLUGIN_H

#include "MergeFunctors.hpp"

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
	template<class Functor>
	void renderGray( const OFX::RenderArguments& args );

	template<class Functor>
	void renderRGBA( const OFX::RenderArguments& args );

public:
	MergePlugin( OfxImageEffectHandle handle );

	inline OFX::Clip* getSrcClipA() const
	{
		return _srcClipA;
	}

	inline OFX::Clip* getSrcClipB() const
	{
		return _srcClipB;
	}

	inline OFX::Clip* getDstClip() const
	{
		return _dstClip;
	}

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::ChoiceParam *_mergeFunction;	///< Functor structure
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClipA;				///< Source image clip A
	OFX::Clip* _srcClipB;				///< Source image clip B
	OFX::Clip* _dstClip;				///< Destination image clip
};

}
}
}

#endif  // MERGE_PLUGIN_H
