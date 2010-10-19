#ifndef _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

template<typename Scalar>
struct TimeShiftProcessParams
{
	Scalar _offset;
};


/**
 * @brief TimeShift plugin
 */
class TimeShiftPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
	TimeShiftPlugin( OfxImageEffectHandle handle );

public:
	TimeShiftProcessParams<Scalar> getProcessParams() const;

	bool getTimeDomain( OfxRangeD& range );
	void getFramesNeeded( const OFX::FramesNeededArguments& args, OFX::FramesNeededSetter& frames );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
	void render( const OFX::RenderArguments& args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc;       ///< Source image clip
	OFX::Clip* _clipDst;       ///< Destination image clip
	OFX::DoubleParam* _offset; ///< Time offset
};

}
}
}

#endif
