#include "ColorDistributionDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <boost/gil/extension/color/distribution.hpp>
#include <boost/gil/extension/typedefs.hpp>

#include <tuttle/plugin/exceptions.hpp>

#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

using namespace boost::gil;

template<class View>
ColorDistributionProcess<View>::ColorDistributionProcess( ColorDistributionPlugin& effect )
	: ImageGilFilterProcessor<View>( effect )
	, _plugin( effect )
{}

template<class View>
void ColorDistributionProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.renderScale );

}

template<class View>
template<class IN>
GIL_FORCEINLINE
void ColorDistributionProcess<View>::processSwitchOut( const EParamDistribution out, const View& src, const View& dst )
{
	using namespace boost::gil;
	switch( out )
	{
		case eParamDistribution_linear:
			transform_pixels_progress( src, dst, transform_pixel_color_distribution_t<IN, boost::gil::colorDistribution::linear>(), *this );
			break;
		case eParamDistribution_sRGB:
			transform_pixels_progress( src, dst, transform_pixel_color_distribution_t<IN, boost::gil::colorDistribution::sRGB>(), *this );
			break;
	}
}

template<class View>
void ColorDistributionProcess<View>::processSwitchInOut( const EParamDistribution in, const EParamDistribution out, const View& src, const View& dst )
{
	using namespace boost::gil;
	switch( in )
	{
		case eParamDistribution_linear:
			processSwitchOut<boost::gil::colorDistribution::linear>( out, src, dst );
			break;
		case eParamDistribution_sRGB:
			processSwitchOut<boost::gil::colorDistribution::sRGB>( out, src, dst );
			break;
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ColorDistributionProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );

	processSwitchInOut( _params._in, _params._out, src, dst );
}

}
}
}
