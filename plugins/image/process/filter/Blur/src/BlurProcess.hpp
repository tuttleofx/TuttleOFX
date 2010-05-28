#ifndef _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_BLUR_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <boost/gil/extension/numeric/kernel.hpp>

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
public:
	typedef double Scalar;
    typedef typename View::value_type Pixel;
    typedef typename image_from_view<View>::type Image;
protected :
    BlurPlugin& _plugin; ///< Rendering plugin

	BlurProcessParams _params; ///< user parameters
	/// @name process temporary values
	/// @{
	boost::gil::kernel_1d<Scalar> _gilKernelX;
	boost::gil::kernel_1d<Scalar> _gilKernelY;
	/// @}
	static const double _convolutionEpsilon = 0.01; ///< arbitrary value...

	static Scalar gaussianValueAt( const Scalar x, const Scalar amplitude, const Scalar yscale = 1, const Scalar xcenter = 0 );
	static boost::gil::kernel_1d<Scalar> buildGaussian1DKernel( const Scalar size );

public:
    BlurProcess( BlurPlugin& effect );

	void setup( const OFX::RenderArguments& args );
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "BlurProcess.tcc"

#endif
