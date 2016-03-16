#ifndef _LENSDISTORTPROCESSPARAMS_HPP_
#define _LENSDISTORTPROCESSPARAMS_HPP_

#include <tuttle/plugin/global.hpp>
#include <terry/globals.hpp>

#include <boost/math/constants/constants.hpp>

namespace tuttle
{
namespace plugin
{
namespace lens
{

using boost::gil::point2;

template <typename T>
T square(const T& v)
{
    return v * v;
}

template <class DistoFunctor>
struct DistoSolver
{
    const DistoFunctor& _distoFunctor;

    DistoSolver(const DistoFunctor& distoFunctor)
        : _distoFunctor(distoFunctor)
    {
    }

    /// Functor to solve Square(disto(radius(p'))) = r^2
    double operator()(double r2) const { return r2 * square(_distoFunctor.distoCoef(_distoFunctor._params, r2)); }
};

/**
 * @brief Solve by bisection the p' radius such that Square(disto(radius(p'))) = r^2
 * @param[in] params: radial distortion parameters
 * @param[in] r2: targeted radius
 * @param[in] functor:
 * @param[in] epsilon: criteria to stop the bisection
 */
template <class DistoFunctor>
double bisectionRadiusSolve(double r2, const DistoSolver<DistoFunctor>& functor, double epsilon = 1e-8)
{
    // Guess plausible upper and lower bound
    double lowerbound = r2, upbound = r2;
    while(functor(lowerbound) > r2)
        lowerbound /= 1.05;
    while(functor(upbound) < r2)
        upbound *= 1.05;

    // Perform a bisection until epsilon accuracy is not reached
    while(epsilon < upbound - lowerbound)
    {
        const double mid = .5 * (lowerbound + upbound);
        if(functor(mid) > r2)
            upbound = mid;
        else
            lowerbound = mid;
    }
    return .5 * (lowerbound + upbound);
}

template <typename F>
struct LensDistortProcessParams
{
    typedef boost::gil::point2<F> Point2;

    /// @group Coordonate system
    /// @{
    Point2 imgSizeSrc;
    Point2 imgCenterSrc;
    Point2 imgCenterDst;
    F normalizeCoef; ///< coef to normalize the pixel values (to work between [-0.5, 0.5],  [0, 1], ...)
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
    F coef1;
    F coef2;
    F coef3;
    F coef4;
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
template <typename F>
class CoordonatesSystem
{
public:
    typedef F Float;
    typedef boost::gil::point2<Float> Point2;

    const LensDistortProcessParams<F>& _params;

public:
    CoordonatesSystem(const LensDistortProcessParams<F>& params)
        : _params(params)
    {
    }
    virtual ~CoordonatesSystem() {}

    /// @group Image coords mapping
    /// @{
    inline Point2 pixelToNormalized(const Point2& p) const
    {
        BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);
        BOOST_STATIC_ASSERT((boost::is_same<F, double>::value));
        Point2 pn(p / _params.normalizeCoef);
        pn.x *= _params.pixelRatio;
        return pn;
    }

    inline Point2 normalizedToPixel(const Point2& pn) const
    {
        Point2 p(pn * _params.normalizeCoef);

        p.x /= _params.pixelRatio;
        return p;
    }

    inline Point2 pixelToCenterNormalized(const Point2& p) const { return pixelToNormalized(p - _params.imgCenterSrc); }

    template <typename F2>
    inline Point2 pixelToCenterNormalized(const boost::gil::point2<F2>& p) const
    {
        Point2 pp(p.x, p.y);

        return pixelToCenterNormalized(pp);
    }

    inline Point2 centerNormalizedToPixel(const Point2& pn) const { return normalizedToPixel(pn) + _params.imgCenterDst; }
    /// @}

    /// @group Lens coords mapping
    /// @{
    inline Point2 pixelToLensCenterNormalized(const Point2& p) const
    {
        return this->pixelToNormalized(p - this->_params.lensCenterDst);
    }

    template <typename F2>
    inline Point2 pixelToLensCenterNormalized(const boost::gil::point2<F2>& p) const
    {
        Point2 pp(p.x, p.y);

        return pixelToLensCenterNormalized(pp);
    }

    inline Point2 lensCenterNormalizedToPixel(const Point2& pn) const
    {
        return this->normalizedToPixel(pn) + this->_params.lensCenterSrc;
    }
    /// @}
};

template <typename F>
struct LensDistortBrown1 : public CoordonatesSystem<F>
{
    LensDistortBrown1(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(this->_params.distort);
        BOOST_ASSERT(this->_params.coef1 >= 0);
        point2<F> pc(this->pixelToLensCenterNormalized(src)); // centered normalized space
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r2 = pc.x * pc.x + pc.y * pc.y;        // distance to center squared
        const F coef = 1.0 + r2 * this->_params.coef1; // distortion coef for current pixel
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // original space
    }
};

template <typename F>
struct LensUndistortBrown1 : public CoordonatesSystem<F>
{
    LensUndistortBrown1(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(!this->_params.distort);
        BOOST_ASSERT(this->_params.coef1 >= 0);
        point2<F> pc(this->pixelToLensCenterNormalized(src));

        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        F r = std::sqrt(pc.x * pc.x + pc.y * pc.y); // distance to center

        // necessary values to calculate
        if(r == 0 || this->_params.coef1 == 0)
        {
            return this->lensCenterNormalizedToPixel(pc);
        }

        // calculate the determinant delta = Q^3 + R^2
        F cQ = -1.0 / (3.0 * this->_params.coef1);
        F cR = -r / (2.f * this->_params.coef1);
        F delta = -cQ * cQ * cQ + cR * cR;
        F coef = 0.0;
        F t;

        // negative or positive determinant
        if(delta > 0)
        {
            coef = std::abs(cR) + std::sqrt(delta);
            coef = -std::pow(coef, 1.0 / 3.0);
            BOOST_ASSERT(coef != 0);
            coef += cQ / coef;
        }
        else if(delta < 0)
        {
            BOOST_ASSERT(cQ >= 0);
            BOOST_ASSERT(cR / (sqrt(cQ * cQ * cQ)) <= 1 && cR / (sqrt(cQ * cQ * cQ)) >= -1);
            t = std::acos(cR / (sqrt(cQ * cQ * cQ)));
            coef = -2.0 * std::sqrt(cQ) * std::cos((t - (boost::math::constants::pi<F>() / 2.0)) / 3.0);
        }
        else
        {
            BOOST_ASSERT(0); // Untreated case..
            return this->lensCenterNormalizedToPixel(pc);
        }

        // get coordinates into distorded image from distorded center
        coef /= r;
        coef = std::abs(coef);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }
};

template <typename F>
struct LensDistortBrown3 : public CoordonatesSystem<F>
{
    LensDistortBrown3(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(this->_params.distort);
        point2<F> pc(this->pixelToLensCenterNormalized(src)); // centered normalized space
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center squared
        // distortion coef for current pixel
        const F coef = distoCoef(this->_params, r2);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // original space
    }

    static F distoCoef(const LensDistortProcessParams<F>& params, F r2)
    {
        const F r4 = r2 * r2;
        const F r6 = r2 * r4;
        // distortion coef for current pixel
        return 1.0 + r2 * params.coef1 + r4 * params.coef2 + r6 * params.coef3;
    }
};

template <typename F>
struct LensUndistortBrown3 : public CoordonatesSystem<F>
{
    LensUndistortBrown3(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(!this->_params.distort);
        point2<F> pc(this->pixelToLensCenterNormalized(src));
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center squared
        const LensDistortBrown3<F> distortion(this->_params);
        const DistoSolver<LensDistortBrown3<F> > distortionSolver(distortion);
        const F coefSolver = bisectionRadiusSolve(r2, distortionSolver);

        const F coef = (r2 == 0) ? 1. : std::sqrt(coefSolver / r2);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }
};

template <typename F>
struct LensDistortPTLens : public CoordonatesSystem<F>
{
    LensDistortPTLens(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(this->_params.distort);
        BOOST_ASSERT(this->_params.coef1 >= 0);
        point2<F> pc(this->pixelToLensCenterNormalized(src)); // centered normalized space
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center squared
        // distortion coef for current pixel
        const F coef = distoCoef(this->_params, r2);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // original space
    }

    static F distoCoef(const LensDistortProcessParams<F>& params, F r2)
    {
        const F r = std::sqrt(r2);
        const F r3 = r * r2;
        // distortion coef for current pixel
        return 1.0 + r * params.coef3 + r2 * params.coef2 + r3 * params.coef1 - params.coef1 - params.coef2 - params.coef3;
    }
};

template <typename F>
struct LensUndistortPTLens : public CoordonatesSystem<F>
{
    LensUndistortPTLens(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(!this->_params.distort);
        BOOST_ASSERT(this->_params.coef1 >= 0);
        point2<F> pc(this->pixelToLensCenterNormalized(src));
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center squared
        const LensDistortPTLens<F> disto(this->_params);
        const DistoSolver<LensDistortPTLens<F> > distoSolver(disto);
        const F coefSolver = bisectionRadiusSolve(r2, distoSolver);

        const F coef = (r2 == 0) ? 1. : std::sqrt(coefSolver / r2);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }
};

template <typename F>
struct LensDistortFisheye : public CoordonatesSystem<F>
{
    LensDistortFisheye(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        point2<F> pc(this->pixelToLensCenterNormalized(src));
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        F r = std::sqrt(pc.x * pc.x + pc.y * pc.y); // distance to center
        if(r == 0)
        {
            point2<F> tmp(src.x, src.y);
            return tmp;
        }
        const F coef = 0.5 * std::tan(r * this->_params.coef1) / (std::tan(0.5 * this->_params.coef1) * r);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }
};

template <typename F>
struct LensUndistortFisheye : public CoordonatesSystem<F>
{
    LensUndistortFisheye(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        point2<F> pc(this->pixelToLensCenterNormalized(src));
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        F r = std::sqrt(pc.x * pc.x + pc.y * pc.y); // distance to center
        if(r == 0 || this->_params.coef1 == 0)
        {
            point2<F> tmp(src.x, src.y);
            return tmp;
        }
        F coef = std::atan(2.0 * r * std::tan(0.5 * this->_params.coef1)) / this->_params.coef1;
        coef /= r;
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }
};

template <typename F>
struct LensDistortFisheye4 : public CoordonatesSystem<F>
{
    LensDistortFisheye4(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(this->_params.distort);
        point2<F> pc(this->pixelToLensCenterNormalized(src)); // centered normalized space
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r = std::sqrt(pc.x * pc.x + pc.y * pc.y); // distance to center squared
        // distortion coef for current pixel
        const F coef = distoCoef(this->_params, r);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // original space
    }

    static F distoCoef(const LensDistortProcessParams<F>& params, F r)
    {
        const double eps = 1e-8;
        const double k1 = params.coef1, k2 = params.coef2, k3 = params.coef3, k4 = params.coef4;
        const double theta = std::atan(r);
        const double theta2 = theta * theta, theta3 = theta2 * theta, theta4 = theta2 * theta2, theta5 = theta4 * theta,
                     theta6 = theta3 * theta3, theta7 = theta6 * theta, theta8 = theta4 * theta4, theta9 = theta8 * theta;
        const double theta_dist = theta + k1 * theta3 + k2 * theta5 + k3 * theta7 + k4 * theta9;
        const double inv_r = r > eps ? 1.0 / r : 1.0;
        const double cdist = r > eps ? theta_dist * inv_r : 1.0;
        return cdist;
    }
};

template <typename F>
struct LensUndistortFisheye4 : public CoordonatesSystem<F>
{
    LensUndistortFisheye4(const LensDistortProcessParams<F>& params)
        : CoordonatesSystem<F>(params)
    {
    }
    template <typename F2>
    inline point2<F> apply(const point2<F2>& src) const
    {
        BOOST_ASSERT(!this->_params.distort);
        point2<F> pc(this->pixelToLensCenterNormalized(src));
        pc.x *= this->_params.postScale.x;
        pc.y *= this->_params.postScale.y;

        const F r = std::sqrt(pc.x * pc.x + pc.y * pc.y); // distance to center squared
        // distortion coef for current pixel
        const F coef = distoCoef(this->_params, r);
        pc.x *= coef;
        pc.y *= coef;

        pc.x *= this->_params.preScale.x;
        pc.y *= this->_params.preScale.y;
        return this->lensCenterNormalizedToPixel(pc); // to the original space
    }

    static F distoCoef(const LensDistortProcessParams<F>& params, F theta_dist)
    {
        const double eps = 1e-8;
        double scale = 1.0;
        if(theta_dist > eps)
        {
            double theta = theta_dist;
            for(int j = 0; j < 10; ++j)
            {
                const double theta2 = theta * theta, theta4 = theta2 * theta2, theta6 = theta4 * theta2,
                             theta8 = theta6 * theta2;
                theta = theta_dist / (1.0 + params.coef1 * theta2 + params.coef2 * theta4 + params.coef3 * theta6 +
                                      params.coef4 * theta8);
            }
            scale = std::tan(theta) / theta_dist;
        }
        return scale;
    }
};
}
}
}

#endif
