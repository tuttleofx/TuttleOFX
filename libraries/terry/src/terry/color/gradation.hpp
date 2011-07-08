#ifndef _BOOST_GIL_COLOR_GRADATION_HPP_
#define _BOOST_GIL_COLOR_GRADATION_HPP_

#include <boost/gil/extension/typedefs.hpp>

namespace terry {
namespace color {

using namespace boost::gil;

/**
 * @todo
 * gradation_convert()
 */
namespace gradation{
struct Linear{};
struct sRGB{};
struct Cineon
{
	Cineon()
		: _blackPoint( 95.0 )
		, _whitePoint( 685.0 )
		, _gammaSensito( 300.0 )
	{}
	Cineon( const double blackPoint, const double whitePoint, const double gammaSensito )
		: _blackPoint( blackPoint )
		, _whitePoint( whitePoint )
		, _gammaSensito( gammaSensito )
	{}
	double _blackPoint;
	double _whitePoint;
	double _gammaSensito;
};
struct Gamma
{
	Gamma()
	: _gamma( 1.0 )
	{}
	Gamma( const double gamma )
	: _gamma(gamma)
	{}
	double _gamma;
};
struct Panalog{};
struct REDLog{};
struct ViperLog{};
struct REDSpace{};
struct AlexaLogC{};
}

/// @brief change the color gradation
template< typename Channel,
          class IN,
          class OUT >
struct channel_color_gradation_t : public std::binary_function<Channel, Channel, Channel>
{
	BOOST_STATIC_ASSERT( sizeof( Channel ) >= 0 ) ; // Error: there is no specialization for this color gradation conversion.
};

template< typename Channel,
          class INOUT >
struct channel_color_gradation_t<Channel, INOUT, INOUT> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;

	const INOUT& _in;
	const INOUT& _out;

	channel_color_gradation_t( const INOUT& in, const INOUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = Channel(src);
	}
};

////////////////////////////////////////////////////////////////////////////////
// sRGB //

/**
 * @brief sRGB to Lin
 * Formula taken from Computational color technology by Kang.
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::sRGB, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::sRGB IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		const T v1 = channel_convert<T>( src );
		T v2;

		/*
		if( src > 0.04045 )
		{
			dst = DstChannel( pow( ( 1.0*src + 0.055 ) / 1.055 , 2.4 ) );
		}
		else
		{
			dst = DstChannel( src / 12.92 );
		}
		*/
		if( v1 > 0.03928 )
		{
			v2 = exp( log( ( v1 + 0.055 ) / 1.055 ) * 2.4 );
		}
		else
		{
			v2 = v1 / 12.92;
		}
		return dst = channel_convert<Channel>( v2 );
	}

};

/**
 * @brief Lin to sRGB
 * Formula taken from Computational color technology by Kang.
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::sRGB> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::sRGB OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		static const double inv_2_4 = 1.0 / 2.4;
		const T v1                  = channel_convert<T>( src );
		T v2;

		/*
		if( src > 0.0031308 )
		{
			dst = DstChannel( 1.055 * pow( src , 1.0 / 2.4 ) - 0.055 );
		}
		else
		{
			dst = DstChannel( 12.92 * src );
		}
		*/
		if( v1 > 0.00304 )
		{
			v2 = 1.055 * exp( log( v1 ) * inv_2_4 ) - 0.055;
		}
		else
		{
			v2 = 12.92 * v1;
		}
		return dst = channel_convert<Channel>( v2 );
	}

};


////////////////////////////////////////////////////////////////////////////////
// Cineon

/**
 * @brief Cineon to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Cineon, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Cineon IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		//double tmpValue = pow( 10 , ( _in._blackPoint - _in._whitepoint ) / 300 );
		//dst = ( pow( 10 , ( src*1024 - _in._whitePoint ) / 300 ) - tmpValue ) / tmpValue;

		return dst = 1.010915615730753 * ( pow( 10 ,( 1023 * src - 685 ) / 300 ) - 0.010797751623277 );
	}
};

/**
 * @brief Lin to Cineon
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::Cineon> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::Cineon OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		//const double tmpValue = pow( 10 , ( _out._blackPoint - _out._whitepoint ) / 300 );
		//dst = ( pow( 10 , ( src*1024 - _out._whitePoint ) / 300 ) - tmpValue ) / tmpValue;

		return dst = 0.00042453028534042214 * ( 300 * log( 0.98920224837672 * src + 0.010797751623277 ) + 1577.270788700921 );
	}
};

////////////////////////////////////////////////////////////////////////////////
// Gamma

/**
 * @brief Gamma to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Gamma, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Gamma IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		if( src == 0 )
		{
			return dst = Channel( 0.0 );
		}
		else if( src > 0.0 )
		{
			// compute gamma value
			//dst = DstChannel( pow( 10 , _value * log10( src ) ) );
			return dst = Channel( pow( src , _in._gamma ) );
		}
		else
		{
			// for negative values, we return a linear conversion
			return dst = Channel( src );
		}
	}
};

/**
 * @brief Lin to Gamma
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::Gamma> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::Gamma OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		if( src == 0 )
		{
			return dst = Channel( 0.0 );
		}
		else if( src > 0.0 )
		{
			// compute gamma value
			return dst = Channel( pow( src , 1.0 / _out._gamma ) );
		}
		else
		{
			// for negative values, we return a linear conversion
			return dst = Channel( src );
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
// Panalog

/**
 * @brief Panalog to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Panalog, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Panalog IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = ( pow ( 10, ( 1023.0 * src - 681.0 ) / 444.0 ) - 0.0408 ) / ( 1.0 - 0.0408 );
	}
};

/**
 * @brief Lin to Panalog
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::Panalog> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::Panalog OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = ( 444.0 * log10( 0.0408 + ( 1 - 0.0408 ) * src ) + 681.0 ) / 1023.0;
	}
};


////////////////////////////////////////////////////////////////////////////////
// REDLog

/**
 * @brief REDLog to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::REDLog, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::REDLog IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = ( pow ( 10, ( 1023.0 * src - 1023.0 ) / 511.0 ) - 0.01 ) / ( 1.0 - 0.01 );
	}
};

/**
 * @brief Lin to REDLog
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::REDLog> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::REDLog OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = ( 511.0 * log10( 0.01 + ( 1 - 0.01 ) * src ) + 1023.0 ) / 1023.0;
	}
};


////////////////////////////////////////////////////////////////////////////////
// ViperLog

/**
 * @brief ViperLog to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::ViperLog, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::ViperLog IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = pow ( 10, ( 1023.0 * src - 1023.0 ) / 500.0 );
	}
};

/**
 * @brief Lin to ViperLog
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::ViperLog> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::ViperLog OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = ( 500.0 * log10( src ) + 1023.0 ) / 1023.0;
	}
};


////////////////////////////////////////////////////////////////////////////////
// REDSpace

/**
 * @brief REDSpace to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::REDSpace, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::REDSpace IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = Channel( src ); // the equation wasn't found actually
	}
};

/**
 * @brief Lin to REDSpace
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::REDSpace> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::REDSpace OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = Channel( src ); // the equation wasn't found actually
	}
};


////////////////////////////////////////////////////////////////////////////////
// AlexaLogC

/**
 * @brief AlexaLogC to Lin
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::AlexaLogC, gradation::Linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::AlexaLogC IN;
	typedef typename gradation::Linear OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = Channel( src ); // the equation wasn't found actually
	}
};

/**
 * @brief Lin to AlexaLogC
 */
template< typename Channel >
struct channel_color_gradation_t<Channel, gradation::Linear, gradation::AlexaLogC> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;
	typedef typename channel_traits<Channel>::const_reference ChannelConstRef;
	typedef typename channel_traits<Channel>::reference ChannelRef;
	typedef typename gradation::Linear IN;
	typedef typename gradation::AlexaLogC OUT;

	const IN& _in;
	const OUT& _out;

	channel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}

	ChannelRef operator()( ChannelConstRef src, ChannelRef dst ) const
	{
		return dst = Channel( src ); // the equation wasn't found actually
	}
};



////////////////////////////////////////////////////////////////////////////////



/**
 * @brief sRGB to custom, using intermediate linear conversion
 */

/*
 * @todo
template< typename Channel,
          class IN,
          class OUT >
struct channel_color_gradation_t : public std::binary_function<Channel, Channel, Channel>
{
	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2,
															const IN& inGradation = In(),
															const OUT& outGradation = OUT() ) const
	{
		channel_color_gradation_t<Channel, IN, gradation::Linear>( ch1, ch2 );
		channel_color_gradation_t<Channel, gradation::Linear, OUT>( ch2, ch2 );
		return ch2;
	}
};
*/

template< typename Pixel,
          class IN,
          class OUT >
struct pixel_color_gradation_t
{
	typedef typename channel_type<Pixel>::type Channel;
	const IN&  _in;
	const OUT& _out;
	
	pixel_color_gradation_t( const IN& in, const OUT& out )
	: _in(in)
	, _out(out)
	{}
	
	Pixel& operator()( const Pixel& p1,
	                   Pixel& p2 ) const
	{
		static_for_each(
				p1, p2,
				channel_color_gradation_t< Channel, IN, OUT >( _in, _out )
			);
		return p2;
	}
};

template< class IN,
          class OUT >
struct transform_pixel_color_gradation_t
{
	const IN  _in;
	const OUT _out;

	transform_pixel_color_gradation_t( const IN& in = IN(), const OUT& out = OUT() )
	: _in(in)
	, _out(out)
	{}

	template< typename Pixel>
	Pixel operator()( const Pixel& p1 ) const
	{
		Pixel p2;
		pixel_color_gradation_t<Pixel, IN, OUT>( _in, _out )( p1, p2 );
		return p2;
	}
};

/// @todo add gradation as parameters
// transform_pixel_color_gradation( sRGB(), Gamma(5.0), View v );

}
}

#endif
