#include "ThinningAlgorithm.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

template<class View>
ThinningProcess<View>::ThinningProcess( ThinningPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void ThinningProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ThinningProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	transform_pixels_locator_progress( this->_srcView, this->_srcPixelRod,
									   this->_dstView, this->_dstPixelRod,
									   procWindowRoW, pixel_thinning_t<View>(), *this );

}

}
}
}
