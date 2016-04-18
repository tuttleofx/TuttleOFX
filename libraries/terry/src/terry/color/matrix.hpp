#ifndef _TERRY_COLOR_MATRIX_HPP_
#define _TERRY_COLOR_MATRIX_HPP_

#include <terry/math/Matrix.hpp>

#include <boost/math/constants/constants.hpp>

namespace terry
{
namespace color
{

// http://www.graficaobscura.com/matrix/index.html

static const double s_redLum = 0.3086;
static const double s_greenLum = 0.6094;
static const double s_blueLum = 0.0820;

/**
 * @brief Make a color scale matrix.
 */
template <class Matrix5x5>
Matrix5x5 scaleMatrix(const typename Matrix5x5::value_type rScale, const typename Matrix5x5::value_type gScale,
                      const typename Matrix5x5::value_type bScale, const typename Matrix5x5::value_type aScale)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = rScale;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = 0.0;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = gScale;
    matrix(1, 2) = 0.0;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = bScale;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = aScale;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Make a contrast matrix.
 */
template <class Matrix5x5>
Matrix5x5 contrastMatrix(const typename Matrix5x5::value_type rScale, const typename Matrix5x5::value_type gScale,
                         const typename Matrix5x5::value_type bScale, const typename Matrix5x5::value_type aScale)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = rScale;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = 0.0;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = gScale;
    matrix(1, 2) = 0.0;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = bScale;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 0.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = (1.0 - rScale) * 0.5;
    matrix(4, 1) = (1.0 - gScale) * 0.5;
    matrix(4, 2) = (1.0 - bScale) * 0.5;
    matrix(4, 3) = (1.0 - aScale) * 0.5;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Make a saturation matrix.
 */
template <class Matrix5x5>
Matrix5x5 saturationMatrix(const typename Matrix5x5::value_type saturation)
{
    typedef typename Matrix5x5::value_type T;

    const T a = (1.0 - saturation) * s_redLum + saturation;
    const T b = (1.0 - saturation) * s_redLum;
    const T c = (1.0 - saturation) * s_redLum;
    const T d = (1.0 - saturation) * s_greenLum;
    const T e = (1.0 - saturation) * s_greenLum + saturation;
    const T f = (1.0 - saturation) * s_greenLum;
    const T g = (1.0 - saturation) * s_blueLum;
    const T h = (1.0 - saturation) * s_blueLum;
    const T i = (1.0 - saturation) * s_blueLum + saturation;

    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = a;
    matrix(0, 1) = b;
    matrix(0, 2) = c;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = d;
    matrix(1, 1) = e;
    matrix(1, 2) = f;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = g;
    matrix(2, 1) = h;
    matrix(2, 2) = i;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0; // no saturation on alpha channel
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Offset r, g, and b.
 */
template <class Matrix5x5>
Matrix5x5 offsetMatrix(const typename Matrix5x5::value_type rOffset, const typename Matrix5x5::value_type gOffset,
                       const typename Matrix5x5::value_type bOffset, const typename Matrix5x5::value_type aOffset)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = 1.0;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = 0.0;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = 1.0;
    matrix(1, 2) = 0.0;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = 1.0;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = rOffset;
    matrix(4, 1) = gOffset;
    matrix(4, 2) = bOffset;
    matrix(4, 3) = aOffset;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief rotate about the x (red) axis.
 */
template <class Matrix5x5>
Matrix5x5 xRotateMatrix(const typename Matrix5x5::value_type rs, const typename Matrix5x5::value_type rc)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = 1.0;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = 0.0;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = rc;
    matrix(1, 2) = rs;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = -rs;
    matrix(2, 2) = rc;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Rotate about the y (green) axis.
 */
template <class Matrix5x5>
Matrix5x5 yRotateMatrix(const typename Matrix5x5::value_type rs, const typename Matrix5x5::value_type rc)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = rc;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = -rs;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = 1.0;
    matrix(1, 2) = 0.0;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = rs;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = rc;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 *	@brief Rotate about the z (blue) axis.
 */
template <class Matrix5x5>
Matrix5x5 zRotateMatrix(const typename Matrix5x5::value_type rs, const typename Matrix5x5::value_type rc)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = rc;
    matrix(0, 1) = rs;
    matrix(0, 2) = 0.0;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = -rs;
    matrix(1, 1) = rc;
    matrix(1, 2) = 0.0;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = 1.0;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Shear z using x and y.
 */
template <class Matrix5x5>
Matrix5x5 zShearMatrix(const typename Matrix5x5::value_type dx, const typename Matrix5x5::value_type dy)
{
    Matrix5x5 matrix(5, 5);

    matrix(0, 0) = 1.0;
    matrix(0, 1) = 0.0;
    matrix(0, 2) = dx;
    matrix(0, 3) = 0.0;
    matrix(0, 4) = 0.0;

    matrix(1, 0) = 0.0;
    matrix(1, 1) = 1.0;
    matrix(1, 2) = dy;
    matrix(1, 3) = 0.0;
    matrix(1, 4) = 0.0;

    matrix(2, 0) = 0.0;
    matrix(2, 1) = 0.0;
    matrix(2, 2) = 1.0;
    matrix(2, 3) = 0.0;
    matrix(2, 4) = 0.0;

    matrix(3, 0) = 0.0;
    matrix(3, 1) = 0.0;
    matrix(3, 2) = 0.0;
    matrix(3, 3) = 1.0;
    matrix(3, 4) = 0.0;

    matrix(4, 0) = 0.0;
    matrix(4, 1) = 0.0;
    matrix(4, 2) = 0.0;
    matrix(4, 3) = 0.0;
    matrix(4, 4) = 1.0;

    return matrix;
}

/**
 * @brief Simple hue rotation. This changes luminance.
 */
template <class Matrix5x5>
Matrix5x5 simpleHueRotateMatrix(const typename Matrix5x5::value_type rotate)
{
    typedef typename Matrix5x5::value_type T;
    using boost::numeric::ublas::prec_prod;

    Matrix5x5 matrix = boost::numeric::ublas::identity_matrix<T>(5);

    /* rotate the grey vector into positive Z */
    static const T xmag = sqrt(2.0);
    static const T xrs = 1.0 / xmag;
    static const T xrc = 1.0 / xmag;
    matrix = prec_prod(matrix, xRotateMatrix<Matrix5x5>(xrs, xrc));

    static const T ymag = sqrt(3.0);
    static const T yrs = -1.0 / ymag;
    static const T yrc = sqrt(2.0) / ymag;

    matrix = prec_prod(matrix, yRotateMatrix<Matrix5x5>(yrs, yrc));

    /* rotate the hue */
    const T zrs = std::sin(rotate * boost::math::constants::pi<T>() / 180.0);
    const T zrc = std::cos(rotate * boost::math::constants::pi<T>() / 180.0);
    matrix = prec_prod(matrix, zRotateMatrix(zrs, zrc));

    /* rotate the grey vector back into place */
    matrix = prec_prod(matrix, yRotateMatrix<Matrix5x5>(-yrs, yrc));
    matrix = prec_prod(matrix, xRotateMatrix<Matrix5x5>(-xrs, xrc));

    return matrix;
}

/**
 * @brief Simple hue rotation. This changes luminance.
 */
template <class Matrix5x5>
void xformpnt(const Matrix5x5& matrix, const typename Matrix5x5::value_type x, const typename Matrix5x5::value_type y,
              const typename Matrix5x5::value_type z, typename Matrix5x5::value_type* tx, typename Matrix5x5::value_type* ty,
              typename Matrix5x5::value_type* tz)
{
    *tx = x * matrix(0, 0) + y * matrix(1, 0) + z * matrix(2, 0) + matrix(4, 0);
    *ty = x * matrix(0, 1) + y * matrix(1, 1) + z * matrix(2, 1) + matrix(4, 1);
    *tz = x * matrix(0, 2) + y * matrix(1, 2) + z * matrix(2, 2) + matrix(4, 2);
}

/**
 * @brief rotate the hue, while maintaining luminance.
 */
template <class Matrix5x5>
Matrix5x5 hueRotateMatrix(const typename Matrix5x5::value_type rotate)
{
    typedef typename Matrix5x5::value_type T;
    using boost::numeric::ublas::prec_prod;

    Matrix5x5 matrix = boost::numeric::ublas::identity_matrix<T>(5);

    // rotate the grey vector into positive Z
    const T xmag = std::sqrt(2.0);
    const T xrs = 1.0 / xmag;
    const T xrc = 1.0 / xmag;
    matrix = prec_prod(matrix, xRotateMatrix<Matrix5x5>(xrs, xrc));

    const T ymag = std::sqrt(3.0);
    const T yrs = -1.0 / ymag;
    const T yrc = std::sqrt(2.0) / ymag;
    matrix = prec_prod(matrix, yRotateMatrix<Matrix5x5>(yrs, yrc));

    T lx;
    T ly;
    T lz;

    // shear the space to make the luminance plane horizontal
    xformpnt(matrix, s_redLum, s_greenLum, s_blueLum, &lx, &ly, &lz);

    BOOST_ASSERT(lz != 0);
    const T zsx = lx / lz;
    const T zsy = ly / lz;
    matrix = prec_prod(matrix, zShearMatrix<Matrix5x5>(zsx, zsy));

    // rotate the hue
    const T zrs = std::sin(rotate * boost::math::constants::pi<T>() / 180.0);
    const T zrc = std::cos(rotate * boost::math::constants::pi<T>() / 180.0);
    matrix = prec_prod(matrix, zRotateMatrix<Matrix5x5>(zrs, zrc));

    // unshear the space to put the luminance plane back
    matrix = prec_prod(matrix, zShearMatrix<Matrix5x5>(-zsx, -zsy));

    // rotate the grey vector back into place
    matrix = prec_prod(matrix, yRotateMatrix<Matrix5x5>(-yrs, yrc));
    matrix = prec_prod(matrix, xRotateMatrix<Matrix5x5>(-xrs, xrc));

    return matrix;
}
}
}

#endif
