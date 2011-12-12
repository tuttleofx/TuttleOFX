#ifndef _TUTTLE_PLUGIN_COLORGRADATION_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADATION_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradation {

/**
 * @brief ColorGradation process
 *
 */
template<class View>
class ColorGradationProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;

protected:
	ColorGradationPlugin&               _plugin;        ///< Rendering plugin
	ColorGradationProcessParams<Scalar> _params;

public:
	ColorGradationProcess( ColorGradationPlugin& effect );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

private:
	template<class IN, class OUT>
	GIL_FORCEINLINE
	void processSwitchAlpha( const bool processAlpha, const View& src, const View& dst, IN gradationIn = IN(), OUT gradationOut = OUT() );

	template <class IN>
	GIL_FORCEINLINE
	void processSwitchOut( const EParamGradation out, const bool processAlpha, const View& src, const View& dst, IN gradationIn = IN() );

	void processSwitchInOut( const EParamGradation in, const EParamGradation out, const bool processAlpha, const View& src, const View& dst );
};

}
}
}

#include "ColorGradationProcess.tcc"

#endif
