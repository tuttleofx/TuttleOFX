#ifndef _TUTTLE_PLUGIN_SOBEL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_SOBEL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/mpl/bool.hpp>

namespace tuttle {
namespace plugin {
namespace sobel {

/**
 * @brief Sobel process
 *
 */
template<class View>
class SobelProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;
protected :
    SobelPlugin&    _plugin;        ///< Rendering plugin
	SobelProcessParams<Scalar> _params; ///< user parameters

public:
    SobelProcess( SobelPlugin& effect );

	void setup( const OFX::RenderArguments& args );
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	void computeGradientDirection( View& dst, boost::mpl::true_ );
	void computeGradientDirection( View& dst, boost::mpl::false_ ){}
};

}
}
}

#include "SobelProcess.tcc"

#endif
