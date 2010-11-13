#ifndef _TUTTLE_PLUGIN_NORMALIZE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace normalize {

/**
 * @brief Normalize process
 *
 */
template<class View>
class NormalizeProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected :
    NormalizePlugin&    _plugin;            ///< Rendering plugin
	NormalizeProcessParams<Scalar> _params; ///< parameters

	/// @brief Processing datas
	/// @{
	Pixel _sMin; ///< min value in source image
	Pixel _dMin; ///< min value in destination image
	Pixel _ratio; ///< scale to go from source to dest
	/// @}

public:
    NormalizeProcess( NormalizePlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "NormalizeProcess.tcc"

#endif
