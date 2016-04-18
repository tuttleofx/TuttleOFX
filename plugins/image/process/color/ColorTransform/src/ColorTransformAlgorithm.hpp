#ifndef _TUTTLE_PLURGBIN_COLORTRANSFORM_ALRGBORITHM_HPP_
#define _TUTTLE_PLURGBIN_COLORTRANSFORM_ALRGBORITHM_HPP_

#include <terry/math/Matrix.hpp>
#include <terry/color/matrix.hpp>
#include <terry/numeric/operations_assign.hpp>

#include <ofxPixels.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/color_base_algorithm.hpp>

#include <boost/numeric/ublas/io.hpp>

#include <boost/mpl/bool.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransform
{

struct ColorTransformParams
{
    double _hueShift;
    double _saturation;
    double _brightnessRGB;
    OfxRGBAColourD _brightness;
    double _pivotRGB;
    OfxRGBAColourD _pivot;
    double _contrastRGB;
    OfxRGBAColourD _contrast;
    double _offsetRGB;
    OfxRGBAColourD _offset;
};

/**
 * @brief
 */
template <class Matrix5x5>
Matrix5x5 colorTransformMatrix(const ColorTransformParams& params)
{
    typedef typename Matrix5x5::value_type T;
    using boost::numeric::ublas::prec_prod;

    Matrix5x5 matrix = boost::numeric::ublas::identity_matrix<T>(5);

    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix, terry::color::hueRotateMatrix<Matrix5x5>(params._hueShift));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;
    matrix = prec_prod(matrix, terry::color::saturationMatrix<Matrix5x5>(params._saturation));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix, terry::color::scaleMatrix<Matrix5x5>(params._brightness.r * params._brightnessRGB,
                                                                    params._brightness.g * params._brightnessRGB,
                                                                    params._brightness.b * params._brightnessRGB,
                                                                    params._brightness.a));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix,
                       terry::color::offsetMatrix<Matrix5x5>( // - Pivot
                           -(params._pivot.r + params._pivotRGB - 1.0f), -(params._pivot.g + params._pivotRGB - 1.0f),
                           -(params._pivot.b + params._pivotRGB - 1.0f), -(params._pivot.a)));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix,
                       terry::color::contrastMatrix<Matrix5x5>( // Contrast
                           params._contrast.r * params._contrastRGB, params._contrast.g * params._contrastRGB,
                           params._contrast.b * params._contrastRGB, params._contrast.a));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix,
                       terry::color::offsetMatrix<Matrix5x5>( // + Pivot
                           params._pivot.r + params._pivotRGB - 1.0f, params._pivot.g + params._pivotRGB - 1.0f,
                           params._pivot.b + params._pivotRGB - 1.0f, params._pivot.a));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    matrix = prec_prod(matrix, terry::color::offsetMatrix<Matrix5x5>(
                                   params._offset.r + params._offsetRGB, params._offset.g + params._offsetRGB,
                                   params._offset.b + params._offsetRGB, params._offset.a));
    std::cout << __LINE__ << " matrix: " << matrix << std::endl;

    return matrix;
}

template <class ChannelColor, class Pixel>
typename boost::gil::channel_type<Pixel>::type
get_color_if_exist_impl(const Pixel& p, const typename boost::gil::channel_type<Pixel>::type&, const boost::mpl::true_)
{
    return boost::gil::get_color(p, ChannelColor());
}
template <class ChannelColor, class Pixel>
typename boost::gil::channel_type<Pixel>::type
get_color_if_exist_impl(const Pixel&, const typename boost::gil::channel_type<Pixel>::type& elseValue,
                        const boost::mpl::false_)
{
    return elseValue;
}
template <class ChannelColor, class Pixel>
typename boost::gil::channel_type<Pixel>::type
get_color_if_exist(const Pixel& p, const typename boost::gil::channel_type<Pixel>::type& elseValue)
{
    using namespace boost::gil;
    typedef typename contains_color<Pixel, ChannelColor>::type hasChannel;

    return get_color_if_exist_impl<ChannelColor, Pixel>(p, elseValue, hasChannel());
}

template <std::size_t y, class Matrix5x5, class Pixel>
inline typename boost::gil::channel_type<Pixel>::type matrixProdChannelImpl(const Matrix5x5& matrix, const Pixel& src)
{
    using namespace boost::gil;
    typedef typename channel_type<Pixel>::type Channel;

    return (matrix(0, y) * get_color_if_exist<red_t>(src, Channel(0.0)) +
            matrix(1, y) * get_color_if_exist<green_t>(src, Channel(0.0)) +
            matrix(2, y) * get_color_if_exist<blue_t>(src, Channel(0.0)) +
            matrix(3, y) * get_color_if_exist<alpha_t>(src, Channel(1.0)) + matrix(4, y));
}

template <class ChannelColor, std::size_t y, class Matrix5x5, class Pixel>
inline void matrixProdChannelAssignImpl(Pixel& dst, const Matrix5x5& matrix, const Pixel& src, const boost::mpl::true_)
{
    using namespace boost::gil;
    get_color(dst, ChannelColor()) = matrixProdChannelImpl<0>(matrix, src);
}
template <class ChannelColor, std::size_t y, class Matrix5x5, class Pixel>
inline void matrixProdChannelAssignImpl(Pixel& dst, const Matrix5x5& matrix, const Pixel& src, const boost::mpl::false_)
{
}
template <class ChannelColor, std::size_t y, class Matrix5x5, class Pixel>
inline void matrixProdChannelAssign(Pixel& dst, const Matrix5x5& matrix, const Pixel& src)
{
    using namespace boost::gil;
    typedef typename contains_color<Pixel, ChannelColor>::type hasChannel;
    matrixProdChannelAssignImpl<ChannelColor, y>(dst, matrix, src, hasChannel());
}

template <class Matrix5x5>
struct MatrixProdPixel
{
    Matrix5x5 _matrix;
    MatrixProdPixel(const Matrix5x5& matrix)
        : _matrix(matrix)
    {
    }

    template <typename Pixel>
    Pixel operator()(const Pixel& src) const
    {
        using namespace boost::gil;
        using namespace terry::color;
        typedef typename channel_type<Pixel>::type Channel;

        Pixel dst;
        matrixProdChannelAssign<red_t, 0>(dst, _matrix, src);
        matrixProdChannelAssign<green_t, 1>(dst, _matrix, src);
        matrixProdChannelAssign<blue_t, 2>(dst, _matrix, src);

        matrixProdChannelAssign<alpha_t, 3>(dst, _matrix, src);
        // or gray
        matrixProdChannelAssign<gray_t, 3>(dst, _matrix, src);

        Channel z = matrixProdChannelImpl<4>(_matrix, src);

        if(z != 0)
        {
            terry::numeric::pixel_divides_scalar_assign_t<Channel, Pixel>()(z, dst);
        }
        return dst;
    }
};
}
}
}

#endif
