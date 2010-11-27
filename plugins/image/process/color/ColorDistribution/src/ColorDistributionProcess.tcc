#include "ColorDistributionDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <boost/gil/extension/typedefs.hpp>

#include <tuttle/plugin/exceptions.hpp>

#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

using namespace boost::gil;

/// @brief change the color distribution
template< typename Channel,
          EParamDistribution IN,
          EParamDistribution OUT >
struct channel_color_distribution_t : public std::binary_function<Channel, Channel, Channel>
{
	BOOST_STATIC_ASSERT( sizeof( Channel ) >= 0 ) ; // Error: there is no specialization for this color distribution conversion.
};

#define TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( COLORDISTRIBUTION ) \
    template < typename Channel > \
    struct channel_color_distribution_t< Channel, COLORDISTRIBUTION, COLORDISTRIBUTION > \
		: public std::binary_function< Channel, Channel, Channel > \
	{ \
		typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1, \
		                                                            typename channel_traits<Channel>::reference ch2 ) const \
		{ \
			BOOST_THROW_EXCEPTION( std::logic_error( "This conversion is an identity conversion, you don't need to do the process." ) ); \
			return ch2 = ch1; \
		} \
	};

/**
 * @brief Lin to sRGB
 * Formula taken from Computational technology by Kang.
 */
template< typename Channel >
struct channel_color_distribution_t<Channel, eParamDistribution_linear, eParamDistribution_sRGB> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;

	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		static const double inv_2_4 = 1.0 / 2.4;
		const T v1                  = channel_convert<T>( ch1 );
		T v2;

		if( v1 > 0.00304 )
		{
			v2 = 1.055 * exp( log( v1 ) * inv_2_4 ) - 0.055;
		}
		else
		{
			v2 = 12.92 * v1;
		}
		return ch2 = channel_convert<Channel>( v2 );
	}

};

/**
 * @brief sRGB to Lin
 * Formula taken from Computational technology by Kang.
 */
template< typename Channel >
struct channel_color_distribution_t<Channel, eParamDistribution_sRGB, eParamDistribution_linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;

	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		const T v1 = channel_convert<T>( ch1 );
		T v2;

		if( v1 > 0.03928 )
		{
			v2 = exp( log( ( v1 + 0.055 ) / 1.055 ) * 2.4 );
		}
		else
		{
			v2 = v1 / 12.92;
		}
		return ch2 = channel_convert<Channel>( v2 );
	}

};

/**
 * @brief sRGB to custom, using intermediate linear conversion
 */
template< typename Channel,
          EParamDistribution OUT >
struct channel_color_distribution_t<Channel, eParamDistribution_sRGB, OUT> : public std::binary_function<Channel, Channel, Channel>
{
	static const EParamDistribution IN = eParamDistribution_sRGB;
	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		channel_color_distribution_t<Channel, IN, eParamDistribution_linear>( ch1, ch2 );
		channel_color_distribution_t<Channel, eParamDistribution_linear, OUT>( ch2, ch2 );
		return ch2;
	}

};

TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( eParamDistribution_linear );
TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( eParamDistribution_sRGB );
//TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( eParamDistribution_cineon );
//TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( eParamDistribution_rec709 );
//TUTTLE_DEFINE_SAME_COLOR_DISTRIBUTION( eParamDistribution_rec601 );

template< typename Pixel,
          EParamDistribution IN,
          EParamDistribution OUT >
struct pixel_color_distribution_t
{
	Pixel& operator()( const Pixel& p1,
	                   Pixel& p2 ) const
	{
		static_for_each( p1, p2,
		                 channel_color_distribution_t< typename channel_type<Pixel>::type,
		                                               IN, OUT >()
		                 );
		return p2;
	}
};

template< EParamDistribution IN,
          EParamDistribution OUT >
struct transform_pixel_color_distribution_t
{
	template< typename Pixel>
	Pixel operator()( const Pixel& p1 ) const
	{
		Pixel p2;
		pixel_color_distribution_t<Pixel, IN, OUT>( )( p1, p2 );
		return p2;
	}
};

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
template <EParamDistribution IN>
GIL_FORCEINLINE
void ColorDistributionProcess<View>::processSwitchOut( const EParamDistribution out, const View& src, const View& dst )
{
	switch( out )
	{
		case eParamDistribution_linear:
			transform_pixels_progress( src, dst, transform_pixel_color_distribution_t<IN, eParamDistribution_linear>(), *this );
			break;
		case eParamDistribution_sRGB:
			transform_pixels_progress( src, dst, transform_pixel_color_distribution_t<IN, eParamDistribution_sRGB>(), *this );
			break;
	}
}

template<class View>
void ColorDistributionProcess<View>::processSwitchInOut( const EParamDistribution in, const EParamDistribution out, const View& src, const View& dst )
{
	switch( in )
	{
		case eParamDistribution_linear:
			processSwitchOut<eParamDistribution_linear>( out, src, dst );
			break;
		case eParamDistribution_sRGB:
			processSwitchOut<eParamDistribution_sRGB>( out, src, dst );
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
