#ifndef _TUTTLE_PLUGIN_COLORDISTRIBUTION_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORDISTRIBUTION_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

/**
 * @brief ColorDistribution process
 *
 */
template<class View>
class ColorDistributionProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;

protected:
	ColorDistributionPlugin&    _plugin;        ///< Rendering plugin
	ColorDistributionProcessParams<Scalar> _params;

public:
	ColorDistributionProcess( ColorDistributionPlugin& effect );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

private:
	template <class IN>
	GIL_FORCEINLINE
	void processSwitchOut( const EParamDistribution out, const View& src, const View& dst );

	void processSwitchInOut( const EParamDistribution in, const EParamDistribution out, const View& src, const View& dst );
};

}
}
}

#include "ColorDistributionProcess.tcc"

#endif
