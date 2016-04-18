#ifndef _TUTTLE_PLUGIN_TPS_HPP_
#define _TUTTLE_PLUGIN_TPS_HPP_

#include "../WarpDefinitions.hpp"

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace warp
{

using namespace boost::gil;
using namespace boost::numeric::ublas;

template <typename SCALAR>
class TPS_Morpher
{
public:
    typedef SCALAR Scalar;
    typedef point2<Scalar> Point2;

    typedef matrix<Scalar> Matrix;
    typedef matrix_row<const Matrix> Const_Matrix_Row;
    typedef matrix_column<const Matrix> Const_Matrix_Col;

    bool _activateWarp;
    std::size_t _nbPoints;
    double _width;
    double _height;
    double _transition;

public:
    TPS_Morpher();

    void setup(const std::vector<Point2> pIn, const std::vector<Point2> pOut, const double regularization,
               const bool applyWarp, const std::size_t width, const std::size_t height, const double transition);

    template <typename S2>
    Point2 operator()(const point2<S2>& pt) const;

private:
    std::vector<Point2> _pIn;
    std::vector<Point2> _pOut;

    Matrix _mat_L;
    Matrix _mat_V;
    Matrix _mat_K;
};
}
}
}

#include "tps.tcc"

#endif
