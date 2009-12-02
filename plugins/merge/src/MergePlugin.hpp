/**
 * @file MergePlugin.hpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */

#ifndef MERGE_PLUGIN_H
#define MERGE_PLUGIN_H

#include "ColorMerge.hpp"

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
	void renderGray( const OFX::RenderArguments& args )
	{
		assert( _dstClip );
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				MergeProcess<gray8_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				MergeProcess<gray16_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				MergeProcess<gray32f_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthCustom:
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
		}
	}
	template<class Functor>
	void renderRGBA( const OFX::RenderArguments& args )
	{
		assert( _dstClip );
		OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();

		// do the rendering
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				MergeProcess<rgba8_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				MergeProcess<rgba16_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				MergeProcess<rgba32f_view_t, Functor> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthCustom:
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
		}
	}

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
