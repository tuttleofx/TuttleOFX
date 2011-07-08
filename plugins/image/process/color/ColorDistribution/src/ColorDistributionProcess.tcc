#include "ColorDistributionDefinitions.hpp"
#include "ColorDistributionProcess.hpp"
#include "ColorDistributionPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/image/gil/copy.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/color/gradation.hpp>
#include <boost/gil/extension/typedefs.hpp>

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
template<class IN, class OUT>
GIL_FORCEINLINE
void ColorDistributionProcess<View>::processSwitchAlpha( const bool processAlpha, const View& src, const View& dst )
{
	using namespace boost::gil;
	if( processAlpha )
	{
		transform_pixels_progress( src, dst, terry::color::transform_pixel_color_gradation_t<IN, OUT>(), *this );
	}
	else
	{
		/// @todo do not apply process on alpha directly inside transform, with a "channel_for_each_if_channel"
		transform_pixels_progress( src, dst, terry::color::transform_pixel_color_gradation_t<IN, OUT>(), *this );

		// temporary solution copy alpha channel
		copy_channel_if_exist<alpha_t>( src, dst );
	}
}

template<class View>
template<class IN>
GIL_FORCEINLINE
void ColorDistributionProcess<View>::processSwitchOut( const EParamDistribution out, const bool processAlpha, const View& src, const View& dst )
{
	using namespace boost::gil;
	switch( out )
	{
		case eParamDistribution_linear:
			processSwitchAlpha<IN, terry::color::gradation::Linear>( processAlpha, src, dst );
			break;
		case eParamDistribution_sRGB:
			processSwitchAlpha<IN, terry::color::gradation::sRGB>( processAlpha, src, dst );
			break;
		case eParamDistribution_cineon:
			processSwitchAlpha<IN, terry::color::gradation::Cineon>( processAlpha, src, dst );
			break;
		case eParamDistribution_gamma:
			processSwitchAlpha<IN, terry::color::gradation::Gamma>( processAlpha, src, dst );
			break;
		case eParamDistribution_panalog:
			processSwitchAlpha<IN, terry::color::gradation::Panalog>( processAlpha, src, dst );
			break;
		case eParamDistribution_REDLog:
			processSwitchAlpha<IN, terry::color::gradation::REDLog>( processAlpha, src, dst );
			break;
		case eParamDistribution_ViperLog:
			processSwitchAlpha<IN, terry::color::gradation::ViperLog>( processAlpha, src, dst );
			break;
		case eParamDistribution_REDSpace:
			processSwitchAlpha<IN, terry::color::gradation::REDSpace>( processAlpha, src, dst );
			break;
		case eParamDistribution_AlexaLogC:
			processSwitchAlpha<IN, terry::color::gradation::AlexaLogC>( processAlpha, src, dst );
			break;
	}
}

template<class View>
void ColorDistributionProcess<View>::processSwitchInOut( const EParamDistribution in, const EParamDistribution out, const bool processAlpha, const View& src, const View& dst )
{
	using namespace boost::gil;
	switch( in )
	{
		case eParamDistribution_linear:
			processSwitchOut<terry::color::gradation::Linear>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_sRGB:
			processSwitchOut<terry::color::gradation::sRGB>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_cineon:
			processSwitchOut<terry::color::gradation::Cineon>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_gamma:
			processSwitchOut<terry::color::gradation::Gamma>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_panalog:
			processSwitchOut<terry::color::gradation::Panalog>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_REDLog:
			processSwitchOut<terry::color::gradation::REDLog>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_ViperLog:
			processSwitchOut<terry::color::gradation::ViperLog>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_REDSpace:
			processSwitchOut<terry::color::gradation::REDSpace>( out, processAlpha, src, dst );
			break;
		case eParamDistribution_AlexaLogC:
			processSwitchOut<terry::color::gradation::AlexaLogC>( out, processAlpha, src, dst );
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

	processSwitchInOut( _params._in, _params._out, _params._processAlpha, src, dst );
}

}
}
}
