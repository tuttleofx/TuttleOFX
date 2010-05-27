#ifndef _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

/**
 * @brief Blur process
 *
 */
template<class View>
class BlurProcess : public ImageGilFilterProcessor<View>
{
protected :
    BlurPlugin&    _plugin;        ///< Rendering plugin

public:
    BlurProcess( BlurPlugin& effect );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "BlurProcess.tcc"

#endif
