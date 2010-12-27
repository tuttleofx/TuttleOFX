#ifndef _LENSDISTORTPROCESSPARAMS_HPP_
#define _LENSDISTORTPROCESSPARAMS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

namespace tuttle {
namespace plugin {
namespace lens {

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
class CoordonatesSystemParams
{
public:
	typedef F Float;
	typedef boost::gil::point2<Float> Point2;

public:
	Point2 _imgSizeSrc;
	Point2 _imgCenterSrc;
	Point2 _imgCenterDst;
	F _imgHalfDiagonal; ///< half diagonal of the image (to work between -0.5 and 0.5)
	F _pixelRatio;

public:
	virtual ~CoordonatesSystemParams() = 0;

	/// @{ conversion functions
	inline Point2 pixelToNormalized( const Point2& p ) const
	{
		BOOST_STATIC_ASSERT( boost::is_floating_point<F>::value );
		BOOST_STATIC_ASSERT( ( boost::is_same<F, double>::value ) );
		Point2 pn( p / _imgHalfDiagonal );
		pn.x *= _pixelRatio;
		return pn;
	}

	inline Point2 normalizedToPixel( const Point2& pn ) const
	{
		Point2 p( pn * _imgHalfDiagonal );

		p.x /= _pixelRatio;
		return p;
	}

	inline Point2 pixelToCenterNormalized( const Point2& p ) const
	{
		return pixelToNormalized( p - _imgCenterSrc );
	}

	template<typename F2>
	inline Point2 pixelToCenterNormalized( const boost::gil::point2<F2>& p ) const
	{
		Point2 pp( p.x, p.y );

		return pixelToCenterNormalized( pp );
	}

	inline Point2 centerNormalizedToPixel( const Point2& pn ) const
	{
		return normalizedToPixel( pn ) + _imgCenterDst;
	}

	/// @}
};

template<typename F>
CoordonatesSystemParams<F>::~CoordonatesSystemParams() {}

template<typename F>
class NormalLensDistortParams : public CoordonatesSystemParams<F>
{
public:
	typedef typename CoordonatesSystemParams<F>::Point2 Point2;

public:
	bool _distort; ///< true : distort, false : undistort
	F _coef1;

	Point2 _lensCenterDst; ///< center of the lens ditortion in the dest image coordinates (in pixels)
	Point2 _lensCenterSrc; ///< center of the lens ditortion in the source image coordinates (in pixels)
	Point2 _postScale;
	Point2 _preScale;

public:
	virtual ~NormalLensDistortParams() {}

	inline Point2 pixelToLensCenterNormalized( const Point2& p ) const
	{
		return pixelToNormalized( p - this->_lensCenterDst );
	}

	template<typename F2>
	inline Point2 pixelToLensCenterNormalized( const boost::gil::point2<F2>& p ) const
	{
		Point2 pp( p.x, p.y );

		return pixelToLensCenterNormalized( pp );
	}

	inline Point2 lensCenterNormalizedToPixel( const Point2& pn ) const
	{
		return normalizedToPixel( pn ) + this->_lensCenterSrc;
	}

};

template<typename F>
class NormalLensUndistortParams : public NormalLensDistortParams<F>
{
public:
	virtual ~NormalLensUndistortParams() {}

};

template<typename F>
class FisheyeLensDistortParams : public NormalLensUndistortParams<F>
{
public:
	F _coef2;

public:
	virtual ~FisheyeLensDistortParams() {}
};

template<typename F>
class FisheyeLensUndistortParams : public FisheyeLensDistortParams<F>
{
public:
	virtual ~FisheyeLensUndistortParams() {}
};

template<typename F>
class AdvancedLensDistortParams : public FisheyeLensUndistortParams<F>
{
public:
	typedef typename CoordonatesSystemParams<F>::Point2 Point2;

public:
	F _squeeze;
	Point2 _asymmetric;

public:
	virtual ~AdvancedLensDistortParams() {}
};

template<typename F>
class AdvancedLensUndistortParams : public AdvancedLensDistortParams<F>
{
public:
	virtual ~AdvancedLensUndistortParams() {}
};

template<typename F>
class LensDistortProcessParams : public AdvancedLensUndistortParams<F>
{
public:
	virtual ~LensDistortProcessParams() {}
};

}
}
}

#endif
