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
protected :
    ColorDistributionPlugin&    _plugin;        ///< Rendering plugin

public:
    ColorDistributionProcess( ColorDistributionPlugin& effect );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorDistributionProcess.tcc"

#endif
