#ifndef _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

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
class TimeShiftPlugin : public ImageEffectGilPlugin
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
	OFX::DoubleParam* _offset; ///< Time offset
};

}
}
}

#endif
