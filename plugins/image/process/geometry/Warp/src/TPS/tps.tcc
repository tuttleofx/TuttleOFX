#include "../WarpPlugin.hpp"
#include "../WarpProcess.hpp"
#include "../WarpDefinitions.hpp"
#include "tps.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/foreach.hpp>

#include <vector>
#include <ostream>

#define TPS_NORMALIZE_COORD

namespace tuttle
{
namespace plugin
{
namespace warp
{

template <class Mat>
inline std::ostream& coutMat(std::ostream& os, const Mat& m)
{
    os << "[" << m.size1() << "," << m.size2() << "]\n";
    for(std::size_t y = 0; y < m.size1(); ++y)
    {
        os << "(";
        for(std::size_t x = 0; x < m.size2(); ++x)
        {
            os << boost::format("%4.3d") % m(y, x);
            if(x < m.size2() - 1)
            {
                os << ",\t";
            }
        }
        os << ")";
        if(y < m.size1() - 1)
        {
            os << ",";
        }
        os << "\n";
    }
    return os;
}

/**
 * @brief Radial basis function: Thin plate spline (a special polyharmonic spline).
 *
 * @f[
 * r^2 * log(r)
 * @f[
 *
 * @param r2 squared distance value
 * @return Computed weight value
 */
inline double base_func(const double r2)
{
    using boost::math::pow;
    if(r2 == 0)
        return 0.0;
    static const double v = 1.0 / (2.0 * log(10.0));
    return r2 * log(r2) * v;
    //	return r2 * log( std::sqrt(r2) ); // same as above solution

    //	return pow<2>(r2) * log( std::sqrt(r2) ); // r^4 * log(r)
    //	return pow<3>( std::sqrt(r2) ); // r^3
    //	return std::sqrt(r2); // r
    //	return r2; // r^2
}

template <typename SCALAR>
TPS_Morpher<SCALAR>::TPS_Morpher()
{
}

/**
 *
 * @param pIn
 * @param pOut
 * @param regularization Amount of "relaxation", 0.0 = exact interpolation
 * @param applyWarp
 * @param nbPoints
 */
template <typename SCALAR>
void TPS_Morpher<SCALAR>::setup(const std::vector<Point2> pIn, const std::vector<Point2> pOut, const double regularization,
                                const bool applyWarp, const std::size_t width, const std::size_t height,
                                const double transition)
{
    using boost::math::pow;

    _width = width;
    _height = height;
    _transition = transition;

#ifdef TPS_NORMALIZE_COORD
    _pIn.reserve(pIn.size());
    _pOut.reserve(pOut.size());
    BOOST_FOREACH(const Point2& p, pIn)
    {
        Point2 np(p.x / _width, p.y / _height);
        _pIn.push_back(np);
    }
    BOOST_FOREACH(const Point2& p, pOut)
    {
        Point2 np(p.x / _width, p.y / _height);
        _pOut.push_back(np);
    }
#else
    _pIn = pIn;
    _pOut = pOut;
#endif

    _nbPoints = _pIn.size();
    _mat_L.resize(_nbPoints + 3, _nbPoints + 3);
    _mat_V.resize(_nbPoints + 3, 2);
    _mat_K.resize(_nbPoints, _nbPoints);
    _activateWarp = applyWarp;

    // TUTTLE_TCOUT_VAR( _pIn.size() );
    // TUTTLE_TCOUT_VAR( _pOut.size() );

    BOOST_ASSERT(_pIn.size() == _pOut.size());

    // Fill K and directly copy values into L
    // K = [ 0      U(r01) ...   U(r0n) ]
    //   = [ U(r10) 0      ...   U(r1n) ]
    //   = [ ...    ...    ...   ...    ]
    //   = [ U(rn0) U(rn1) ...   0      ]
    //
    // K.size = n x n
    for(std::size_t y = 0; y < _nbPoints; ++y)
    {
        const Point2& point_i = _pIn[y];
        for(std::size_t x = 0; x < _nbPoints; ++x)
        {
            if(y == x)
            {
                _mat_L(y, x) = _mat_K(y, x) = regularization;
                // diagonal: reqularization parameters (lambda * a^2)
                //				_mat_L( i, i ) = _mat_K( i, i ) = regularization /** (a*a)*/;
            }
            else
            {
                const Point2& point_j = _pIn[x];
                const Scalar sum = pow<2>(point_i.x - point_j.x) + pow<2>(point_i.y - point_j.y);
                _mat_L(y, x) = _mat_K(y, x) = base_func(sum);
            }
        }
    }

    // fill L with P and trans(P)
    //
    // P = [ 1   x1   y1  ]
    //     [ 1   x2   y2  ]
    //     [ ... ...  ... ]
    //     [ 1   xn   yn  ]
    //
    // P.size = n x 3
    for(std::size_t i = 0; i < _nbPoints; ++i)
    {
        const Point2& pt = _pIn[i];
        _mat_L(i, _nbPoints + 0) = 1.0;
        _mat_L(i, _nbPoints + 1) = pt.x;
        _mat_L(i, _nbPoints + 2) = pt.y;

        _mat_L(_nbPoints + 0, i) = 1.0;
        _mat_L(_nbPoints + 1, i) = pt.x;
        _mat_L(_nbPoints + 2, i) = pt.y;
    }

    // L = [ K        |  P  ]
    //     [          | 000 ]
    //     [ trans(P) | 000 ]
    //     [          | 000 ]
    //
    // L.size = (n+3) x (n+3)
    for(std::size_t i = 0; i < 3; ++i)
    {
        for(std::size_t j = 0; j < 3; ++j)
        {
            _mat_L(_nbPoints + i, _nbPoints + j) = 0.0;
        }
    }

    // Fill V
    // V = [ x1' x2' ... xn' 000 ]
    //     [ y1' y2' ... yn' 000 ]
    //
    // V.size = 2 x (n+3)
    // here we manipulate trans(V)
    for(std::size_t i = 0; i < _nbPoints; ++i)
    {
        const Point2& pIn = _pIn[i];
        const Point2& pOut = _pOut[i];
        _mat_V(i, 0) = pOut.x - pIn.x;
        _mat_V(i, 1) = pOut.y - pIn.y;
        // TUTTLE_TCOUT_VAR2( pOut.x, pIn.x );
        // TUTTLE_TCOUT_VAR2( pOut.y, pIn.y );
    }

    _mat_V(_nbPoints + 0, 0) = _mat_V(_nbPoints + 1, 0) = _mat_V(_nbPoints + 2, 0) = 0.0;
    _mat_V(_nbPoints + 0, 1) = _mat_V(_nbPoints + 1, 1) = _mat_V(_nbPoints + 2, 1) = 0.0;

    // TUTTLE_TCOUT("");
    // TUTTLE_TCOUT( "mtx_v" );
    // coutMat( std::cout, _mat_V );
    // TUTTLE_TCOUT("");
    // TUTTLE_TCOUT( "mtx_orig_k" );
    // coutMat( std::cout, _mat_K );
    // TUTTLE_TCOUT("");
    // TUTTLE_TCOUT( "mtx_l" );
    // coutMat( std::cout, _mat_L );
    // TUTTLE_TCOUT("");

    // Solve the linear system "inplace"
    permutation_matrix<Scalar> P(_nbPoints + 3);
    //	matrix<Scalar> x( _nbPoints + 3, 2 );

    lu_factorize(_mat_L, P);
    lu_substitute(_mat_L, P, _mat_V);

    // TUTTLE_TCOUT_X( 10, "-");
    // TUTTLE_TCOUT( "mtx_v" );
    // coutMat( std::cout, _mat_V );
    // TUTTLE_TCOUT("");
    // TUTTLE_TCOUT( "mtx_l" );
    // coutMat( std::cout, _mat_L );
    // TUTTLE_TCOUT("");
    // TUTTLE_TCOUT_X( 80, "_");
}

template <typename SCALAR>
template <typename S2>
typename TPS_Morpher<SCALAR>::Point2 TPS_Morpher<SCALAR>::operator()(const point2<S2>& pt) const
{
    using boost::math::pow;
    if(!_activateWarp || _nbPoints <= 1)
    {
        return Point2(pt.x, pt.y);
    }
#ifdef TPS_NORMALIZE_COORD
    const Point2 npt(pt.x / _width, pt.y / _height);
#else
    const Point2 npt(pt.x, pt.y);
#endif

    // Nombre de colonnes de la matrice K
    const std::size_t m = _mat_K.size2();

    double dx = _mat_V(m + 0, 0) + _mat_V(m + 1, 0) * npt.x + _mat_V(m + 2, 0) * npt.y;
    double dy = _mat_V(m + 0, 1) + _mat_V(m + 1, 1) * npt.x + _mat_V(m + 2, 1) * npt.y;

    Const_Matrix_Col mat_Vx(_mat_V, 0);
    Const_Matrix_Col mat_Vy(_mat_V, 1);

    std::vector<point2<double> >::const_iterator it_out = _pOut.begin();
    typename Const_Matrix_Col::const_iterator it_Vx(mat_Vx.begin());
    typename Const_Matrix_Col::const_iterator it_Vy(mat_Vy.begin());

    // std::cout<<"DX fin "<<dx<<"  "<<dy<<std::endl;

    for(std::size_t i = 0; i < m; ++i, ++it_out, ++it_Vx, ++it_Vy)
    {
        const double d = base_func(pow<2>(it_out->x - npt.x) + pow<2>(it_out->y - npt.y));
        dx += (*it_Vx) * d;
        dy += (*it_Vy) * d;
    }
    dx *= _transition;
    dy *= _transition;
#ifdef TPS_NORMALIZE_COORD
    return Point2((npt.x + dx) * _width, (npt.y + dy) * _height);
#else
    return Point2(npt.x + dx, npt.y + dy);
#endif
}
}
}
}
