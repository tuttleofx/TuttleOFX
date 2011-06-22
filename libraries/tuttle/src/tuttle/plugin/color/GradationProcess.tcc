
namespace tuttle {
namespace plugin {
namespace color {

/*-------------------------------- method to process pixels values ------------------------------------*/

struct computeFromLinear
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeToLinear
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeFromSRGB
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		if( src > 0.04045 )
		{
			dst = DstChannel( pow( ( 1.0*src + 0.055 ) / 1.055 , 2.4 ) );
		}
		else
		{
			dst = DstChannel( 1.0 * src / 12.92 );
		}
	}
};

struct computeToSRGB
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		if( src > 0.0031308 )
		{
			dst = DstChannel( 1.055 * pow( src , 1.0 / 2.4 ) - 0.055 );
		}
		else
		{
			dst = DstChannel( 12.92 * src );
		}
	}
};

struct computeFromCineon
{
	const double _blackPoint;
	const double _whitePoint;
	const double _gammaSensito;

	computeFromCineon( const double blackPoint, const double whitePoint, const double gammaSensito ) : _blackPoint( blackPoint ), _whitePoint( whitePoint ), _gammaSensito( gammaSensito ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		//double tmpValue = pow( 10 , ( _blackPoint - _whitepoint ) / 300 );
		//dst = ( pow( 10 , ( src*1024 - _whitePoint ) / 300 ) - tmpValue ) / tmpValue;

		dst = 1.010915615730753 * ( pow( 10 ,( 1023 * src - 685 ) / 300 ) - 0.010797751623277 );
	}
};

struct computeToCineon
{
	const double _blackPoint;
	const double _whitePoint;
	const double _gammaSensito;

	computeToCineon( const double blackPoint, const double whitePoint, const double gammaSensito ) : _blackPoint( blackPoint ), _whitePoint( whitePoint ), _gammaSensito( gammaSensito ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		//double tmpValue = pow( 10 , ( _blackPoint - _whitepoint ) / 300 );
		//dst = ( pow( 10 , ( src*1024 - _whitePoint ) / 300 ) - tmpValue ) / tmpValue;

		dst = 0.00042453028534042214 * ( 300 * log( 0.98920224837672 * src + 0.010797751623277 ) + 1577.270788700921 );
	}
};

struct computeFromGamma
{
	const double _value;

	computeFromGamma( const double value ) : _value( value ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		if( src == 0 )
			dst = DstChannel( 0.0 );
		if( src > 0.0 )
		{
			// compute gamma value
			//dst = DstChannel( pow( 10 , _value * log10( src ) ) );
			dst = DstChannel( pow( src , _value ) );
		}
		else
		{
			// for negative values, we return a linear conversion
			dst = DstChannel( src );
		}
	}
};

struct computeToGamma
{
	const double _value;

	computeToGamma( const double value ) : _value( value ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		if( src == 0 )
			dst = DstChannel( 0.0 );
		if( src > 0.0 )
		{
			// compute gamma value
			dst = DstChannel( pow( src , 1.0 / _value ) );
		}
		else
		{
			// for negative values, we return a linear conversion
			dst = DstChannel( src );
		}
	}
};

// equation are axctracted from Nuke, in Project Settings
struct computeFromPanalog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( pow ( 10, ( 1023.0 * src - 681.0 ) / 444.0 ) - 0.0408 ) / ( 1.0 - 0.0408 );
	}
};

struct computeToPanalog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( 444.0 * log10( 0.0408 + ( 1 - 0.0408 ) * src ) + 681.0 ) / 1023.0;
	}
};

struct computeFromRedLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( pow ( 10, ( 1023.0 * src - 1023.0 ) / 511.0 ) - 0.01 ) / ( 1.0 - 0.01 );
	}
};

struct computeToRedLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( 511.0 * log10( 0.01 + ( 1 - 0.01 ) * src ) + 1023.0 ) / 1023.0;
	}
};

struct computeFromViperLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = pow ( 10, ( 1023.0 * src - 1023.0 ) / 500.0 );
	}
};

struct computeToViperLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( 500.0 * log10( src ) + 1023.0 ) / 1023.0;
	}
};

struct computeFromRedSpace
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

struct computeToRedSpace
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

struct computeFromAlexaLogC
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

struct computeToAlexaLogC
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

}
}
}
