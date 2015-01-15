#include "ColorMatrixAlgorithm.hpp"

#include <terry/numeric/matrix.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle {
namespace plugin {
namespace colorMatrix {

template<class View>
ColorMatrixProcess<View>::ColorMatrixProcess( ColorMatrixPlugin &effect )
: ImageGilFilterProcessor<View>( effect, eImageOrientationIndependant )
, _plugin( effect )
{
}

template<class View>
void ColorMatrixProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ColorMatrixProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::algorithm;

	typedef boost::numeric::ublas::bounded_matrix<float, 4, 4 > Matrix44;

	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	const OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
	                                   procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
	                                          procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                                          procWindowSize.x, procWindowSize.y );

	terry::algorithm::transform_pixels_progress(
		src,
		dst,
		terry::numeric::pixel_matrix44_multiply_t<Pixel, Matrix44, Pixel>( _params.matrixMix ),
		*this
		);
}

}
}
}
