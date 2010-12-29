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
template<class SView, class DView>
class SobelProcess : public ImageGilFilterProcessor<SView, DView>
{
public:
	typedef float Scalar;
	typedef typename SView::point_t Point;
	typedef typename DView::value_type DPixel;

protected :
    SobelPlugin&    _plugin;        ///< Rendering plugin
	SobelProcessParams<Scalar> _params; ///< user parameters

	DPixel _pixelZero;

public:
    SobelProcess( SobelPlugin& effect );

	void setup( const OFX::RenderArguments& args );
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class ProcPixelGray>
	void computeXPass2( DView& dst, const Point& proc_tl, boost::mpl::true_ );
	template<class ProcPixelGray>
	void computeXPass2( DView& dst, const Point& proc_tl, boost::mpl::false_ )
	{
		BOOST_THROW_EXCEPTION( exception::Bug() );
	}

	template<class ProcPixelGray>
	void computeYPass2( DView& dst, const Point& proc_tl, boost::mpl::true_ )
	{
		using namespace boost::gil;
		correlate_cols<ProcPixelGray>(
			kth_channel_view<1>( this->_srcView ),
			_params._yKernelGaussianDerivative,
			kth_channel_view<1>(dst),
			proc_tl,
			_params._boundary_option );
	}
	template<class ProcPixelGray>
	void computeYPass2( DView& dst, const Point& proc_tl, boost::mpl::false_ )
	{
		BOOST_THROW_EXCEPTION( exception::Bug() );
	}

	void computeGradientDirection( DView& dst, boost::mpl::true_ );
	void computeGradientDirection( DView& dst, boost::mpl::false_ ){}
};

}
}
}

#include "SobelProcess.tcc"

#endif
