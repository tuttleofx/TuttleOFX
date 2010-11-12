#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsProcess.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/typedefs.hpp>
#include <tuttle/plugin/param/gilColor.hpp>

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations2.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations3.hpp>
#include <boost/gil/extension/toolbox/hsl.hpp>

#include <boost/units/pow.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/find.hpp>


namespace boost {
namespace gil {

namespace details {

template <typename Colorspace, typename ChannelType>
// models pixel concept
struct without_alpha_channel_impl_t
{
	typedef pixel<ChannelType, layout<Colorspace> > Pixel;
	typedef Pixel PixelR;

	BOOST_STATIC_ASSERT( ( !contains_color<Pixel, alpha_t>::value ) );

	PixelR operator ()( const Pixel& src ) const
	{
		return src;
	}

};

template <typename ChannelType>
// models pixel concept
struct without_alpha_channel_impl_t<rgba_t, ChannelType>
{
	typedef rgba_t Colorspace;
	typedef pixel<ChannelType, layout<Colorspace> > Pixel;
	typedef pixel<ChannelType, layout<rgb_t> > PixelR;

	BOOST_STATIC_ASSERT( ( contains_color<Pixel, alpha_t>::value ) );

	PixelR operator ()( const Pixel& src ) const
	{
		PixelR dst;

		get_color( dst, red_t() )   = get_color( src, red_t() );
		get_color( dst, green_t() ) = get_color( src, green_t() );
		get_color( dst, blue_t() )  = get_color( src, blue_t() );
		return dst;
	}

};

}

template <typename Pixel>
// models pixel concept
struct without_alpha_channel_t
{
	typedef typename channel_type<Pixel>::type ChannelType;
	typedef typename color_space_type<Pixel>::type Colorspace;

	typedef details::without_alpha_channel_impl_t<Colorspace, ChannelType> Do;
	typedef typename Do::PixelR PixelR;

	PixelR operator ()( const Pixel& src ) const
	{
		return Do() ( src );
	}

};

GIL_DEFINE_ALL_TYPEDEFS( 64, hsl )
GIL_DEFINE_ALL_TYPEDEFS( 64s, hsl )
GIL_DEFINE_ALL_TYPEDEFS( 64f, hsl )

}
}

namespace tuttle {
namespace plugin {
namespace imageStatistics {

using namespace boost::gil;

template<typename T>
T standard_deviation( const T v_sum, const T v_sum_p2, const std::size_t nb )
{
	using namespace boost::units;
	return std::sqrt( v_sum_p2 / nb - pow<2>( v_sum / nb ) );
}

template<typename Pixel>
Pixel pixel_standard_deviation( const Pixel& v_sum, const Pixel& v_sum_p2, const std::size_t nb )
{
	using namespace boost::gil;
	Pixel res;
	for( int i = 0; i < num_channels<Pixel>::type::value; ++i )
	{
		res[i] = standard_deviation( v_sum[i], v_sum_p2[i], nb );
	}
	return res;
}

/**
 * @brief In probability theory and statistics, kurtosis is a measure of the
 * "peakedness" of the probability distribution of a real-valued random variable.
 *
 * Higher kurtosis means more of the variance is the result of infrequent extreme deviations,
 * as opposed to frequent modestly sized deviations.
 */
template<typename T>
T kurtosis( const T v_mean, const T v_standard_deviation, const T v_sum, const T v_sum_p2, const T v_sum_p3, const T v_sum_p4, const std::size_t nb )
{
	using namespace boost::units;
	return ( ( v_sum_p4 - 4.0 * v_mean * v_sum_p3 + 6.0 * pow<2>( v_mean ) * v_sum_p2 ) / nb - 3.0 * pow<4>( v_mean ) ) /
	       ( pow<4>( v_standard_deviation ) ) - 3.0;
}

template<typename Pixel>
Pixel pixel_kurtosis( const Pixel& v_mean, const Pixel& v_standard_deviation, const Pixel& v_sum, const Pixel& v_sum_p2, const Pixel& v_sum_p3, const Pixel& v_sum_p4, const std::size_t nb )
{
	using namespace boost::gil;
	Pixel res;
	for( int i = 0; i < num_channels<Pixel>::type::value; ++i )
	{
		res[i] = kurtosis( v_mean[i], v_standard_deviation[i], v_sum[i], v_sum_p2[i], v_sum_p3[i], v_sum_p4[i], nb );
	}
	return res;
}

/**
 * @brief In probability theory and statistics, skewness is a measure of the
 * asymmetry of the probability distribution of a real-valued random variable.
 *
 * A zero value indicates that the values are relatively evenly distributed
 * on both sides of the mean, typically but not necessarily implying
 * a symmetric distribution.
 */
template<typename T>
T skewness( const T v_mean, const T v_standard_deviation, const T v_sum, const T v_sum_p2, const T v_sum_p3, const std::size_t nb )
{
	using namespace boost::units;
	return ( ( v_sum_p3 - 3.0 * v_mean * v_sum_p2 ) / nb + 2.0 * pow<3>( v_mean ) ) / pow<3>( v_standard_deviation );
}

template<typename Pixel>
Pixel pixel_skewness( const Pixel& v_mean, const Pixel& v_standard_deviation, const Pixel& v_sum, const Pixel& v_sum_p2, const Pixel& v_sum_p3, const std::size_t nb )
{
	using namespace boost::gil;
	Pixel res;
	for( int i = 0; i < num_channels<Pixel>::type::value; ++i )
	{
		res[i] = skewness( v_mean[i], v_standard_deviation[i], v_sum[i], v_sum_p2[i], v_sum_p3[i], nb );
	}
	return res;
}

template<class Pixel>
struct OutputParams
{
	OutputParams()
	{
		using namespace boost::gil;
		pixel_zeros_t<Pixel>( )( _average );
		pixel_zeros_t<Pixel>( )( _channelMin );
		pixel_zeros_t<Pixel>( )( _channelMax );
		pixel_zeros_t<Pixel>( )( _luminosityMin );
		pixel_zeros_t<Pixel>( )( _luminosityMax );
		pixel_zeros_t<Pixel>( )( _kurtosis );
		pixel_zeros_t<Pixel>( )( _skewness );
	}

	Pixel _average;
	Pixel _channelMin;
	Pixel _channelMax;
	Pixel _luminosityMin;
	Pixel _luminosityMax;
	Pixel _kurtosis;
	Pixel _skewness;
};

template<class View, typename CType = boost::gil::bits64f>
struct ComputeOutputParams
{

	typedef typename View::value_type Pixel;
	typedef typename color_space_type<View>::type Colorspace;
	typedef pixel<typename channel_type<View>::type, layout<gray_t> > PixelGray; // grayscale pixel type (using the input channel_type)
	typedef pixel<CType, layout<Colorspace> > CPixel; // the pixel type use for computation (using input colorspace)

	typedef OutputParams<CPixel> Output;

	Output operator()( const View& image, ImageStatisticsPlugin& plugin )
	{
		OutputParams<CPixel> output;

		const std::size_t nbPixels = image.width() * image.height();

		// declare values and init
		Pixel firstPixel = *image.begin(); // for initialization only
		PixelGray firstPixelGray;
		color_convert( firstPixel, firstPixelGray );

		Pixel channelMin            = firstPixel;
		Pixel channelMax            = firstPixel;
		Pixel luminosityMin         = firstPixel;
		PixelGray luminosityMinGray = firstPixelGray;
		Pixel luminosityMax         = firstPixel;
		PixelGray luminosityMaxGray = firstPixelGray;

		CPixel sum;
		CPixel sum_p2;
		CPixel sum_p3;
		CPixel sum_p4;
		pixel_zeros_t<CPixel>( )( sum );
		pixel_zeros_t<CPixel>( )( sum_p2 );
		pixel_zeros_t<CPixel>( )( sum_p3 );
		pixel_zeros_t<CPixel>( )( sum_p4 );

		for( int y = 0;
		     y < image.height();
		     ++y )
		{
			typename View::x_iterator src_it = image.x_at( 0, y );
			CPixel lineAverage;
			pixel_zeros_t<CPixel>( )( lineAverage );

			for( int x = 0;
			     x < image.width();
			     ++x, ++src_it )
			{
				CPixel pix;
				pixel_assigns_t<Pixel, CPixel>( )( * src_it, pix ); // pix = src_it;

				CPixel pix_p2;
				CPixel pix_p3;
				CPixel pix_p4;

				pixel_assigns_t<CPixel, CPixel>( )( pixel_pow_t<CPixel, 2>( )( pix ), pix_p2 ); // pix_p2 = pow<2>( pix );
				pixel_assigns_t<CPixel, CPixel>( )( pixel_multiplies_t<CPixel, CPixel, CPixel>( )( pix, pix_p2 ), pix_p3 ); // pix_p3 = pix * pix_p2;
				pixel_assigns_t<CPixel, CPixel>( )( pixel_multiplies_t<CPixel, CPixel, CPixel>( )( pix_p2, pix_p2 ), pix_p4 ); // pix_p4 = pix_p2 * pix_p2;

				pixel_plus_assign_t<CPixel, CPixel>( )( pix, sum ); // sum += pix;
				pixel_plus_assign_t<CPixel, CPixel>( )( pix_p2, sum_p2 ); // sum_p2 += pix_p2;
				pixel_plus_assign_t<CPixel, CPixel>( )( pix_p3, sum_p3 ); // sum_p3 += pix_p3;
				pixel_plus_assign_t<CPixel, CPixel>( )( pix_p4, sum_p4 ); // sum_p4 += pix_p4;

				// search min for each channel
				pixel_assign_min_t<Pixel, Pixel>( )( * src_it, channelMin );
				// search max for each channel
				pixel_assign_max_t<Pixel, Pixel>( )( * src_it, channelMax );

				PixelGray grayCurrentPixel; // current pixel in gray colorspace
				color_convert( * src_it, grayCurrentPixel );

				// search min luminosity
				if( get_color( grayCurrentPixel, gray_color_t() ) < get_color( luminosityMinGray, gray_color_t() ) )
				{
					luminosityMin     = *src_it;
					luminosityMinGray = grayCurrentPixel;
				}
				// search max luminosity
				if( get_color( grayCurrentPixel, gray_color_t() ) > get_color( luminosityMaxGray, gray_color_t() ) )
				{
					luminosityMax     = *src_it;
					luminosityMaxGray = grayCurrentPixel;
				}
			}
		}

		output._channelMin    = channelMin;
		output._channelMax    = channelMax;
		output._luminosityMin = luminosityMin;
		output._luminosityMax = luminosityMax;

		CPixel stdDeriv = pixel_standard_deviation( sum, sum_p2, nbPixels );
		output._average  = pixel_divides_scalar_t<CPixel, double>() ( sum, nbPixels );
		output._kurtosis = pixel_kurtosis( output._average, stdDeriv, sum, sum_p2, sum_p3, sum_p4, nbPixels );
		output._skewness = pixel_skewness( output._average, stdDeriv, sum, sum_p2, sum_p3, nbPixels );

		return output;
	}

};


template <typename OutputParamsRGBA, typename OutputParamsHSL>
void setOutputParams( const OutputParamsRGBA& outputParamsRGBA, const OutputParamsHSL& outputParamsHSL, const OfxTime time, ImageStatisticsPlugin& plugin )
{
	setRGBAParamValuesAtTime( *plugin._paramOutputAverage, time, outputParamsRGBA._average );
	//	COUT_VAR4( outputParamsRGBA._average[0], outputParamsRGBA._average[1], outputParamsRGBA._average[2], outputParamsRGBA._average[3] );
	setRGBAParamValuesAtTime( *plugin._paramOutputChannelMin, time, outputParamsRGBA._channelMin );
	//	COUT_VAR4( outputParamsRGBA._channelMin[0], outputParamsRGBA._channelMin[1], outputParamsRGBA._channelMin[2], outputParamsRGBA._channelMin[3] );
	setRGBAParamValuesAtTime( *plugin._paramOutputChannelMax, time, outputParamsRGBA._channelMax );
	setRGBAParamValuesAtTime( *plugin._paramOutputLuminosityMin, time, outputParamsRGBA._luminosityMin );
	setRGBAParamValuesAtTime( *plugin._paramOutputLuminosityMax, time, outputParamsRGBA._luminosityMax );
	setRGBAParamValuesAtTime( *plugin._paramOutputKurtosis, time, outputParamsRGBA._kurtosis );
	setRGBAParamValuesAtTime( *plugin._paramOutputSkewness, time, outputParamsRGBA._skewness );

	set012ParamValuesAtTime( plugin._paramOutputAverageHSL, time, outputParamsHSL._average );
	//	COUT_VAR4( outputParamsHSL._average[0], outputParamsHSL._average[1], outputParamsHSL._average[2], outputParamsHSL._average[3] );
	set012ParamValuesAtTime( *plugin._paramOutputChannelMinHSL, time, outputParamsHSL._channelMin );
	//	COUT_VAR4( outputParamsHSL._channelMin[0], outputParamsHSL._channelMin[1], outputParamsHSL._channelMin[2], outputParamsHSL._channelMin[3] );
	set012ParamValuesAtTime( *plugin._paramOutputChannelMaxHSL, time, outputParamsHSL._channelMax );
	set012ParamValuesAtTime( *plugin._paramOutputLuminosityMinHSL, time, outputParamsHSL._luminosityMin );
	set012ParamValuesAtTime( *plugin._paramOutputLuminosityMaxHSL, time, outputParamsHSL._luminosityMax );
	set012ParamValuesAtTime( *plugin._paramOutputKurtosisHSL, time, outputParamsHSL._kurtosis );
	set012ParamValuesAtTime( *plugin._paramOutputSkewnessHSL, time, outputParamsHSL._skewness );
}

template<class View>
ImageStatisticsProcess<View>::ImageStatisticsProcess( ImageStatisticsPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void ImageStatisticsProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;

	ImageGilFilterProcessor<View>::setup( args );
	_processParams = _plugin.getProcessParams( this->_srcPixelRod );

	View image = subimage_view( this->_srcView,
	                            _processParams._rect.x1,
	                            _processParams._rect.y1,
	                            _processParams._rect.x2 - _processParams._rect.x1,
	                            _processParams._rect.y2 - _processParams._rect.y1 );

	typedef ComputeOutputParams<View, boost::gil::bits64f> ComputeRGBA;
	typename ComputeRGBA::Output outputRGBA = ComputeRGBA() ( image, this->_plugin );

	typedef pixel<typename channel_type<View>::type, layout<hsl_t> > HSLPixel;
	typedef color_converted_view_type<View, HSLPixel> HSLConverter;
	typedef ComputeOutputParams<typename HSLConverter::type, boost::gil::bits64f> ComputeHSL;
	typename ComputeHSL::Output outputHSL = ComputeHSL() ( color_converted_view<HSLPixel>( image ), this->_plugin );

	setOutputParams( outputRGBA, outputHSL, args.time, this->_plugin );

	switch( _processParams._chooseOutput )
	{
		case eParamChooseOutputSource:
			break;
		case eParamChooseOutputAverage:
			color_convert( outputRGBA._average, _outputPixel );
			break;
		case eParamChooseOutputChannelMin:
			color_convert( outputRGBA._channelMin, _outputPixel );
			break;
		case eParamChooseOutputChannelMax:
			color_convert( outputRGBA._channelMax, _outputPixel );
			break;
		case eParamChooseOutputLuminosityMin:
			color_convert( outputRGBA._luminosityMin, _outputPixel );
			break;
		case eParamChooseOutputLuminosityMax:
			color_convert( outputRGBA._luminosityMax, _outputPixel );
			break;
	}
}

/**
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void ImageStatisticsProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	if( _processParams._chooseOutput == eParamChooseOutputSource )
	{
		for( int y = procWindowOutput.y1;
		     y < procWindowOutput.y2;
		     ++y )
		{
			typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
			typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
			for( int x = procWindowOutput.x1;
			     x < procWindowOutput.x2;
			     ++x, ++src_it, ++dst_it )
			{
				( *dst_it ) = ( *src_it );
			}
			if( this->progressForward() )
				return;
		}
	}
	else
	{
		for( int y = procWindowOutput.y1;
		     y < procWindowOutput.y2;
		     ++y )
		{
			typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
			for( int x = procWindowOutput.x1;
			     x < procWindowOutput.x2;
			     ++x, ++dst_it )
			{
				( *dst_it ) = _outputPixel;
			}
			if( this->progressForward() )
				return;
		}
	}
}

}
}
}
