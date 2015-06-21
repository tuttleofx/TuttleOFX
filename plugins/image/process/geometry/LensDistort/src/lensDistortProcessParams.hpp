#ifndef _LENSDISTORTPROCESSPARAMS_HPP_
#define _LENSDISTORTPROCESSPARAMS_HPP_

#include <tuttle/plugin/global.hpp>
#include <terry/globals.hpp>

#include <boost/math/constants/constants.hpp>

namespace tuttle {
namespace plugin {
namespace lens {

using boost::gil::point2;

template<typename F>
struct LensDistortProcessParams
{
	typedef boost::gil::point2<F> Point2;

	/// @group Coordonate system
	/// @{
	Point2 imgSizeSrc;
	Point2 imgCenterSrc;
	Point2 imgCenterDst;
	F imgHalfDiagonal; ///< half diagonal of the image (to work between -0.5 and 0.5)
	F pixelRatio;
	/// @}

	/// @group Lens
	/// @{
	bool distort; ///< true : distort, false : undistort

	Point2 lensCenterDst; ///< center of the lens ditortion in the dest image coordinates (in pixels)
	Point2 lensCenterSrc; ///< center of the lens ditortion in the source image coordinates (in pixels)
	Point2 postScale;
	Point2 preScale;
	/// @}

	/// @group Brown
	/// @{
	F brown1;
	F brown2;
	F brown3;
	/// @}

	/// @group Advanced
	/// @{
	F squeeze;
	Point2 asymmetric;
	/// @}
};

/**
 * @brief Contains functions to map coordinates between :
 *  * canonical coordinates system (ofx)
 * e.g PAL D1
 *     ----------------------------- 720.0, 576.0
 *     |                           |
 *     |                           |
 *     |                           |
 *     |                           |
 *     |                           |
 *     -----------------------------
 * 0.0, 0.0
 *  * centered normalized coordinate system (custom, only depending on image width)
 *     ----------------------------- 0.5, 0.5
 *     |                           |
 *     |---------------------------|
 *     |                           |
 *     |                           |
 *     |         0.0, 0.0          |
 *     |                           |
 *     |                           |
 *     |---------------------------|
 *     |                           |
 *     -----------------------------
 * -0.5, -0.5
 */
template<typename F>
class CoordonatesSystem
{
public:
	typedef F Float;
	typedef boost::gil::point2<Float> Point2;

	const LensDistortProcessParams<F>& _params;

public:
	CoordonatesSystem(const LensDistortProcessParams<F>& params)
	: _params(params)
	{}
	virtual ~CoordonatesSystem() {}

	/// @group Image coords mapping
	/// @{
	inline Point2 pixelToNormalized( const Point2& p ) const
	{
		BOOST_STATIC_ASSERT( boost::is_floating_point<F>::value );
		BOOST_STATIC_ASSERT( ( boost::is_same<F, double>::value ) );
		Point2 pn( p / _params.imgHalfDiagonal );
		pn.x *= _params.pixelRatio;
		return pn;
	}

	inline Point2 normalizedToPixel( const Point2& pn ) const
	{
		Point2 p( pn * _params.imgHalfDiagonal );

		p.x /= _params.pixelRatio;
		return p;
	}

	inline Point2 pixelToCenterNormalized( const Point2& p ) const
	{
		return pixelToNormalized( p - _params.imgCenterSrc );
	}

	template<typename F2>
	inline Point2 pixelToCenterNormalized( const boost::gil::point2<F2>& p ) const
	{
		Point2 pp( p.x, p.y );

		return pixelToCenterNormalized( pp );
	}

	inline Point2 centerNormalizedToPixel( const Point2& pn ) const
	{
		return normalizedToPixel( pn ) + _params.imgCenterDst;
	}
	/// @}

	/// @group Lens coords mapping
	/// @{
	inline Point2 pixelToLensCenterNormalized( const Point2& p ) const
	{
		return this->pixelToNormalized( p - this->_params.lensCenterDst );
	}

	template<typename F2>
	inline Point2 pixelToLensCenterNormalized( const boost::gil::point2<F2>& p ) const
	{
		Point2 pp( p.x, p.y );

		return pixelToLensCenterNormalized( pp );
	}

	inline Point2 lensCenterNormalizedToPixel( const Point2& pn ) const
	{
		return this->normalizedToPixel( pn ) + this->_params.lensCenterSrc;
	}
	/// @}
};

template<typename F>
struct LensDistortBrown1 : public CoordonatesSystem<F>
{
	LensDistortBrown1(const LensDistortProcessParams<F>& params)
	: CoordonatesSystem<F>(params)
	{}
	template<typename F2>
	inline point2<F> apply( const point2<F2>& src ) const
	{
		BOOST_ASSERT( this->_params.distort );
		BOOST_ASSERT( this->_params.brown1 >= 0 );
		point2<F> pc( this->pixelToLensCenterNormalized( src ) ); // centered normalized space
		pc *= this->_params.postScale;

		const F r2   = pc.x * pc.x + pc.y * pc.y; // distance to center squared
		const F coef = 1.0 + r2 * this->_params.brown1; // distortion coef for current pixel
		pc.x *= coef;
		pc.y *= coef;

		pc *= this->_params.preScale;
		return this->lensCenterNormalizedToPixel( pc ); // original space
	}
};

template<typename F>
struct LensUndistortBrown1 : public CoordonatesSystem<F>
{
	LensUndistortBrown1(const LensDistortProcessParams<F>& params)
	: CoordonatesSystem<F>(params)
	{}
	template<typename F2>
	inline point2<F> apply( const point2<F2>& src ) const
	{
		BOOST_ASSERT( !this->_params.distort );
		BOOST_ASSERT( this->_params.brown1 >= 0 );
		point2<F> pc( this->pixelToLensCenterNormalized( src ) );

		pc *= this->_params.postScale;

		F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center

		// necessary values to calculate
		if( r == 0 || this->_params.brown1 == 0 )
		{
			return this->lensCenterNormalizedToPixel( pc );
		}

		// calculate the determinant delta = Q^3 + R^2
		F cQ    = -1.0 / ( 3.0 * this->_params.brown1 );
		F cR    = -r / ( 2.f * this->_params.brown1 );
		F delta = -cQ * cQ * cQ + cR * cR;
		F coef  = 0.0;
		F t;

		// negative or positive determinant
		if( delta > 0 )
		{
			coef = std::abs( cR ) + std::sqrt( delta );
			coef = -std::pow( coef, 1.0 / 3.0 );
			BOOST_ASSERT( coef != 0 );
			coef += cQ / coef;
		}
		else if( delta < 0 )
		{
			BOOST_ASSERT( cQ >= 0 );
			BOOST_ASSERT( cR / ( sqrt( cQ * cQ * cQ ) ) <= 1 && cR / ( sqrt( cQ * cQ * cQ ) ) >= -1 );
			t    = std::acos( cR / ( sqrt( cQ * cQ * cQ ) ) );
			coef = -2.0 * std::sqrt( cQ ) * std::cos( ( t - (boost::math::constants::pi<F>() / 2.0) ) / 3.0 );
		}
		else
		{
			BOOST_ASSERT( 0 ); // Untreated case..
			return this->lensCenterNormalizedToPixel( pc );
		}

		// get coordinates into distorded image from distorded center
		coef /= r;
		coef  = std::abs( coef );

		pc *= coef;
		pc *= this->_params.preScale;
		return this->lensCenterNormalizedToPixel( pc ); // to the original space
	}
};


template<typename F>
struct LensDistortFisheye : public CoordonatesSystem<F>
{
	LensDistortFisheye(const LensDistortProcessParams<F>& params)
	: CoordonatesSystem<F>(params)
	{}
	template<typename F2>
	inline point2<F> apply( const point2<F2>& src ) const
	{
		point2<F> pc( this->pixelToLensCenterNormalized( src ) );
		pc *= this->_params.postScale;

		// F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center
		// F coef = this->_params.postScale * ( 1.0 + r2 * this->_params.brown1 + ( r2 * r2 ) * this->_params.brown2 ); // distortion coef for current pixel
		// pc.x *= coef;
		// pc.y *= coef;
		F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center
		if( r == 0 )
		{
			point2<F> tmp( src.x, src.y );
			return tmp;
		}
		F coef = 0.5 * std::tan( r * this->_params.brown1 ) / ( std::tan( 0.5 * this->_params.brown1 ) * r );

		pc *= coef;
		pc *= this->_params.preScale;
		return this->lensCenterNormalizedToPixel( pc ); // to the original space
	}
};

template<typename F>
struct LensUndistortFisheye : public CoordonatesSystem<F>
{
	LensUndistortFisheye(const LensDistortProcessParams<F>& params)
	: CoordonatesSystem<F>(params)
	{}
	template<typename F2>
	inline point2<F> apply( const point2<F2>& src ) const
	{
		point2<F> pc( this->pixelToLensCenterNormalized( src ) );
		pc *= this->_params.postScale;

		F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center
		if( r == 0 || this->_params.brown1 == 0 )
		{
			point2<F> tmp( src.x, src.y );
			return tmp;
		}
		F coef = std::atan( 2.0 * r * std::tan( 0.5 * this->_params.brown1 ) ) / this->_params.brown1;
		//if(rd<0) rd = -rd;
		coef /= r;

		pc *= coef;
		pc *= this->_params.preScale;
		return this->lensCenterNormalizedToPixel( pc ); // to the original space
	}
};


template<typename F>
struct LensDistortAdvanced : public CoordonatesSystem<F>
{
	LensDistortAdvanced(const LensDistortProcessParams<F>& params)
	: CoordonatesSystem<F>(params)
	{}
	template<typename F2>
	inline point2<F> apply( const point2<F2>& src ) const
	{
		point2<F> pc( this->pixelToLensCenterNormalized( src ) );
		pc *= this->_params.postScale;

		F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center
		F r4 = r2 * r2;
		pc.x *= ( 1.0 + r2 * this->_params.brown1 + r4 * this->_params.brown2 );
		pc.y *= ( 1.0 + ( r2 * this->_params.brown1 + r4 * this->_params.brown2 ) / this->_params.squeeze );

		pc *= this->_params.preScale;
		return this->lensCenterNormalizedToPixel( pc ); // to the original space
	}
};


}
}
}

#endif
