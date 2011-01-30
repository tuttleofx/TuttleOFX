#ifndef _TUTTLE_PLUGIN_INTERACT_FRAME_HPP_
#define _TUTTLE_PLUGIN_INTERACT_FRAME_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace interact {

struct IFrame
{
	virtual ~IFrame()                                      = 0;
	virtual bool     isEnabled() const                    = 0;
	virtual OfxRectD getFrame( const OfxTime time ) const = 0;
};

struct FrameClip : virtual public IFrame
{
	const OFX::Clip& _clip;
	FrameClip( const OFX::Clip* clip )
		: _clip( *clip )
	{}

	bool isEnabled() const
	{
		return _clip.isConnected();
	}

	OfxRectD getFrame( const OfxTime time ) const
	{
		static const OfxRectD frame = { 0, 1, 0, 1 };

		if( _clip.isConnected() )
			return _clip.getCanonicalRod( time );
		return frame;
	}

};

struct FrameOptionalClip : virtual public IFrame
{
	const OFX::Clip& _optionalClip;
	const OFX::Clip& _clip;
	FrameOptionalClip( const OFX::Clip* optionalClip, const OFX::Clip* clip )
		: _optionalClip( *optionalClip )
		, _clip( *clip )
	{}

	bool isEnabled() const
	{
		return _optionalClip.isConnected() || _clip.isConnected();
	}

	OfxRectD getFrame( const OfxTime time ) const
	{
		static const OfxRectD frame = { 0, 1, 0, 1 };

		if( _optionalClip.isConnected() )
			return _optionalClip.getCanonicalRod( time );
		if( _clip.isConnected() )
			return _clip.getCanonicalRod( time );
		return frame;
	}

};

}
}
}

#endif

