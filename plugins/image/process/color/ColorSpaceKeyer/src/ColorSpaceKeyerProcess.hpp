#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_

#include "SelectionAverage.hpp"
#include "GeodesicForm.hpp"
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

struct Compute_alpha_pixel
{
    bool _isOutputBW; // is output black & white (or alpha channel)
    GeodesicForm& _dataColor; // color geodesic form
    GeodesicForm& _dataSpill; // spill geodesic form

    Compute_alpha_pixel(bool isOutputBW, GeodesicForm& dataC, GeodesicForm& dataS)
        : _isOutputBW(isOutputBW)
        , _dataColor(dataC)
        , _dataSpill(dataS)
    {
    }

    template <typename Pixel>
    Pixel operator()(const Pixel& p)
    {
        using namespace boost::gil;

        double alpha = 0.0; // define current pixel alpha (1 by default)
        Ofx3DPointD testPoint; // initialize test point
        testPoint.x = p[0]; // x == red
        testPoint.y = p[1]; // y == green
        testPoint.z = p[2]; // z == blue

        if(_dataSpill.isIntoBoundingBox(testPoint)) // if current pixel is into spill bounding box
        {
            if(_dataColor.isIntoBoundingBox(testPoint)) // bounding box test (process optimization)
            {
                if(_dataColor.isPointIntoGeodesicForm(testPoint)) // if current pixel is into the color geodesic form
                {
                    alpha = 1.0; // change alpha to 1
                }
            }
            if(alpha != 1.0 && _dataSpill.isPointIntoGeodesicForm(testPoint))
            {
                // intersections test
                if(_dataColor.testIntersection2(testPoint, false, true)) // normal intersection
                {
                    // computes vectors
                    Ofx3DPointD vectMax;
                    Ofx3DPointD vectMin;
                    // compute min vector
                    vectMin.x = testPoint.x - _dataColor._intersectionPoint.x; // x value
                    vectMin.y = testPoint.y - _dataColor._intersectionPoint.y; // y value
                    vectMin.z = testPoint.z - _dataColor._intersectionPoint.z; // z value
                    // compute max vector
                    vectMax.x = _dataSpill._intersectionPoint.x - _dataColor._intersectionPoint.x; // x value
                    vectMax.y = _dataSpill._intersectionPoint.y - _dataColor._intersectionPoint.y; // x value
                    vectMax.z = _dataSpill._intersectionPoint.z - _dataColor._intersectionPoint.z; // x value
                    // compute norms
                    double normMin, normMax; // initialize
                    normMin = vectMin.x * vectMin.x; // add x*x
                    normMin += vectMin.y * vectMin.y; // add y*y
                    normMin += vectMin.z * vectMin.z; // add z*z
                    normMin = sqrt(normMin); // compute norm minimal

                    normMax = vectMax.x * vectMax.x; // add x*x
                    normMax += vectMax.y * vectMax.y; // add y*y
                    normMax += vectMax.z * vectMax.z; // add z*z
                    normMax = sqrt(normMax); // compute norm maximal

                    // compute alpha value
                    alpha = normMin / normMax;
                }
            }
        }
        alpha = 1 - alpha; // black is transparent and white is opaque
        Pixel ret; // declare returned pixel
        if(_isOutputBW) // output is gray scale image
        {
            gray32f_pixel_t inter;     // need a gray_pixel
            inter[0] = alpha;          // recopy value (black or white)
            color_convert(inter, ret); // convert gray_pixel to rgba_pixel
        }
        else // output is alpha channel
        {
            for(unsigned int i = 0; i < boost::gil::num_channels<Pixel>::type::value - 1; ++i) // all RGB channels (not
                                                                                               // alpha)
                ret[i] = p[i]; // recopy r,g and b channels
            ret[3] = alpha; // fill alpha channel up
        }
        return ret;
    }
};

/**
 * @brief ColorSpaceKeyer process
 *
 */
template <class View>
class ColorSpaceKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    ColorSpaceKeyerPlugin& _plugin;               ///< Rendering plugin
    ColorSpaceKeyerProcessParams<Scalar> _params; ///< parameters
public:
    ColorSpaceKeyerProcess(ColorSpaceKeyerPlugin& effect);
    void setup(const OFX::RenderArguments& args);
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ColorSpaceKeyerProcess.tcc"

#endif
