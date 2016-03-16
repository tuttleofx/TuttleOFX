#ifndef SELECTIONAVERAGE_HPP
#define SELECTIONAVERAGE_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include "GeodesicForm.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

typedef boost::gil::rgba32f_view_t SView;
typedef boost::gil::rgba32f_pixel_t SPixel;
typedef std::vector<float, OfxAllocator<float> > DataVector;

// Functor used to compute average on color clip selection
template <class View, typename CType = boost::gil::bits64f>
struct ComputeAverage
{
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::color_space_type<View>::type Colorspace;
    typedef boost::gil::pixel<CType, boost::gil::layout<Colorspace> >
        CPixel; // the pixel type use for computation (using input colorspace)

    CPixel operator()(const View& image)
    {
        using namespace boost::gil;
        using namespace terry;
        using namespace terry::numeric;
        std::size_t nbPixels = 0;
        CPixel sum; // sum of each pixel
        pixel_zeros_t<CPixel>()(sum); // set sum to 0

        for(int y = 0; // for each lines
            y < image.height(); ++y)
        {
            typename View::x_iterator src_it = image.x_at(0, y);
            for(int x = 0; x < image.width(); ++x, ++src_it) // for each pixels in a line
            {
                CPixel pix; // initialize a container pixel
                pixel_assigns_t<Pixel, CPixel>()(*src_it, pix); // pix = src_it;

                if(pix[3] == 1) // if current pixel is selected (alpha channel == 1)
                {
                    pixel_plus_assign_t<CPixel, CPixel>()(pix, sum); // sum += pix;
                    ++nbPixels; // increments number of selected pixel
                }
            }
        }
        if(nbPixels != 0) // if any selected pixel has been added
            sum = pixel_divides_scalar_t<CPixel, double>()(sum, nbPixels); // compute average average (sum/nbPixels)
        return sum; // return average (or (0,0,0) pixel)
    }
};

// Functor : extend the geodesicForm with selection (only alpha channel)
struct Pixel_extend_GeodesicForm
{
    // Arguments
    GeodesicForm& _data; // buffer to fill up

    // Constructor
    Pixel_extend_GeodesicForm(GeodesicForm& data)
        : _data(data)
    {
    }
    // Operator ()
    template <typename Pixel>
    Pixel operator()(const Pixel& p)
    {
        using namespace boost::gil;
        // RGB only
        if(p[3] == 1) // current pixel is in the selection
        {
            // construct current point
            Ofx3DPointD point; // initialize
            point.x = p[0]; //(x == red)
            point.y = p[1]; //(y == green)
            point.z = p[2]; //(z == blue)

            // extend geodesicForm
            _data.extendOnePoint(point); // extends geodesic form (point is already into geodesic form checked)
        }
        return p;
    }
};

class SelectionAverage
{
public:
    // Arguments
    Ofx3DPointD _averageValue; // Average of the selection color clip (selection)
    OfxTime _time; // current time in sequence
public:
    // Constructor (initialize value to 0)
    SelectionAverage(OfxTime time);

    // Draw average value on screen (cross)
    void draw();
    // Generate average from color selection clip
    bool computeAverageSelection(OFX::Clip* clipColor, const OfxPointD& renderScale);

    // Extends a geodesic form with the selection
    void extendGeodesicForm(OFX::Clip* clipColor, const OfxPointD& renderScale, GeodesicForm& geodesicForm);
};
}
}
}

#endif /* SELECTIONAVERAGE_HPP */
