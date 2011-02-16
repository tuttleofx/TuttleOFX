#ifndef _TUTTLE_PLUGIN_CROP_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CROP_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

/**
 * @brief Base class
 *
 */
template<class View>
class CropProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	CropPlugin&          _plugin;        ///< Rendering plugin
	CropProcessParams<Pixel>    _params;        ///< process parameters

public:
	CropProcess<View>( CropPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "CropProcess.tcc"

#endif

