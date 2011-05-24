
/*-------------------------------- method to process pixels values ------------------------------------*/

struct computeLinear
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeSRGB
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

struct computeCineon
{
	const double _blackPoint;
	const double _whitePoint;
	const double _gammaSensito;

	computeCineon( const double blackPoint, const double whitePoint, const double gammaSensito ) : _blackPoint( blackPoint ), _whitePoint( whitePoint ), _gammaSensito( gammaSensito ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		//double tmpValue = pow( 10 , ( _blackPoint - _whitepoint ) / 300 );
		//dst = ( pow( 10 , ( src*1024 - _whitePoint ) / 300 ) - tmpValue ) / tmpValue;

		dst = 1.010915615730753 * ( pow( 10 ,(1023*src-685)/300 ) - 0.010797751623277);
	}
};

struct computeGamma
{
	const double _value;

	computeGamma( const double value ) : _value( value ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		if( src == 0 )
			dst = DstChannel( 0.0 );
		if( src > 0.0 )
		{
			// compute gamma value
			dst = DstChannel( pow( 10 , _value * log10( src ) ) );
		}
		else
		{
			// for negative values, we return a linear conversion
			dst = DstChannel( src );
		}
	}
};

// equation are axctracted from Nuke, in Project Settings
struct computePanalog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( pow ( 10, ( 1023 * src - 681 ) / 444 ) - 0.0408 ) / ( 1 - 0.0408 );
	}
};

struct computeRedLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = ( pow ( 10, ( 1023 * src - 1023 ) / 511 ) - 0.01 ) / ( 1 - 0.01 );
	}
};

struct computeViperLog
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = pow ( 10, ( 1023 * src - 1023 ) / 500 );
	}
};

struct computeRedSpace
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

struct computeAlexaLogC
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src ); // the equation wasn't found actually
	}
};

/*------------------------- methods to convert input to lin --------------------------------*/
template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::linear, LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeLinear() );
	return true;
}


template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::sRGB , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeSRGB() );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::cineon , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeCineon( inColorSpace.law.blackPoint, inColorSpace.law.whitePoint, inColorSpace.law.gammaSensito ) );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::gamma , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeGamma( inColorSpace.law.value ) );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::panalog , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computePanalog() );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::redLog , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRedLog() );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::viperLog , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeViperLog() );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::redSpace , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRedSpace() );
	return true;
}

template < typename SrcP, typename DstP, typename LayoutIn >
bool convertGradationLaw( const ttlc_colorspace< GradationLaw::alexaLogC , LayoutIn > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeAlexaLogC() );
	return true;
}
