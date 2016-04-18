#ifndef _TUTTLE_PLUGIN_BEZIER_HPP_
#define _TUTTLE_PLUGIN_BEZIER_HPP_

#include "../WarpPlugin.hpp"
#include "../WarpProcess.hpp"
#include "../WarpDefinitions.hpp"
#include "../WarpOverlayInteract.hpp"

#include <sstream>
#include <tuttle/plugin/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <tuttle/plugin/opengl/gl.h>

#include <vector>

namespace tuttle
{
namespace plugin
{
namespace warp
{
namespace bezier
{

using namespace boost::gil;
using namespace boost::numeric::ublas;

point2<double> barycentre(const std::vector<point2<double> >& pts, const double t);

void tracerPoint(const point2<double>& p);

void drawBezier(const std::vector<point2<double> >& tabPts, const std::size_t nbBezier, const double r, const double v,
                const double b);

/**
 * @param[in] pts List of four points [A, A tangent, B tangent, B]
 * @param[in] subdivs Number of subdivisions
 * @param[out] outputPts Output list of bezier points
 */
void bezierSubdivide(const std::vector<point2<double> >& pts, const std::size_t subdivs,
                     std::vector<point2<double> >& outputPts);
}
}
}
}

#endif
