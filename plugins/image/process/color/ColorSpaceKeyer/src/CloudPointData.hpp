#ifndef CLOUDPOINTDATA_HPP
#define CLOUDPOINTDATA_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include "GeodesicForm.hpp"
#include "SelectionAverage.hpp"
#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/channel_algorithm.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>

#include <boost/foreach.hpp>

#include <vector>
#include <set>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

typedef std::vector<float, OfxAllocator<float> > DataVector;
typedef boost::gil::rgba32f_view_t SView;
typedef boost::gil::rgba32f_pixel_t SPixel;

template <typename PixelRef1, // models pixel concept
          typename PixelRef2> // models pixel concept
struct pixel_is_lesser_t
{
    GIL_FORCEINLINE
    bool operator()(const PixelRef1& p1, const PixelRef2& p2) const
    {
        for(int v = 0; v < 3 /*boost::gil::num_channels<Pixel>::type::value*/; ++v)
        {
            if(p1[v] < p2[v])
                return true;
            if(p1[v] > p2[v])
                return false;
        }
        return false;
    }
};

// Functor : copy image into a buffer (ignore alpha channel)
struct Pixel_copy
{
    // Arguments
    DataVector& _data; // buffer to fill up
    bool _isSelection; // is current operation on selection (only take good alpha pixels)

    // Constructor
    Pixel_copy(DataVector& data, bool isSelection = false)
        : _data(data)
        , _isSelection(isSelection)
    {
    }
    // Operator ()
    template <typename Pixel>
    Pixel operator()(const Pixel& p)
    {
        using namespace boost::gil;
        // is current operation for selectionVBO
        if(_isSelection) // test if current operation is to selection VBO
        {
            if(p[3] != 1) // if current pixel is not selected (alpha != 1)
                return p; // stop treatment
        }
        // recopy channels
        for(int v = 0; v < 3 /*boost::gil::num_channels<Pixel>::type::value*/; ++v)
        {
            const float val = boost::gil::channel_convert<boost::gil::bits32f>(p[v]); // capt channel (red, green or blue)
            _data.push_back(val);                                                     // add value to buffer data
        }
        return p;
    }
};

// Functor copy image (with discretization)
template <typename Pixel>
struct Pixel_copy_discretization
{
    typedef std::set<SPixel, tuttle::plugin::colorSpaceKeyer::pixel_is_lesser_t<SPixel, SPixel> > PixelSet;

    // Arguments
    DataVector& _data; // buffer to fill up
    int _nbStep;       // discretization step
    float _step;       // discretization value
    PixelSet _setData; // we use to set to prevent double same data many times
    bool _isSelection; // is current operation to selection VBO

    // Constructor
    Pixel_copy_discretization(DataVector& data, int nbStep, bool isSelection = false)
        : _data(data)
        , _nbStep(nbStep)
        , _isSelection(isSelection)
    {
        _step = (float)(1 / (float)(nbStep - 1)); // compute discretization value
    }

    // Operator ()
    Pixel operator()(const Pixel& p)
    {
        Pixel add; // used to put data in the std::set
        using namespace boost::gil;
        // test if current operation is to selection VBO
        if(_isSelection) // test
        {
            if(p[3] != 1) // if current pixel is not selected (alpha != 1)
                return p; // stop treatment
        }
        // recopy channels
        for(int v = 0; v < 3 /*boost::gil::num_channels<Pixel>::type::value*/; ++v) // We don't want work with alpha channel
        {
            double val = p[v]; // capt channel (red, green or blue)

            bool placedInVector = false; // values is not in the vector yet
            int iteration = 0;           // initialize index to 0
            while(!placedInVector)       // place round value in the vector
            {
                float previousValue = iteration * _step;
                float nextValue = (iteration + 1) * _step;
                if(val <= previousValue)
                {
                    add[v] = previousValue; // add value to pixel data
                    placedInVector = true;  // value is put in the vector
                }
                else if(val < nextValue)
                {
                    add[v] = nextValue;    // add value to pixel data
                    placedInVector = true; // value is put in the vector
                }
                ++iteration; // increments indice
            }
        }
        _setData.insert(add); // insert current pixel into the set (doubles are not allowed)
        return p;
    }

    // Convert _setData values to _data values
    void convertSetDataToVectorData()
    {
        // iterator creation
        BOOST_FOREACH(const SPixel& p, _setData) // iterator used to scan the set
        {
            _data.push_back(p[0]); // red channel copy into vector
            _data.push_back(p[1]); // green channel copy into vector
            _data.push_back(p[2]); // blue channel copy into vector
        }
    }
};

class CloudPointData
{

protected:
    // VBO class (used to add an OpenGL VBO to scene)
    class VBO
    {
    public:
        // VBO class (for create / remove / draw VBOs)
        VBO(const void* data, int dataSize, GLenum usage)
            : _id(0)
            , _size(0)
            , _color(0)
            , _colorDifferent(0) // Constructor
        {
            createVBO(data, dataSize, usage);
        }
        // Constructor
        VBO()
            : _id(0)
            , _size(0)
            , _color(0)
            , _colorDifferent(0)
        {
        }
        // Destructor
        ~VBO() { deleteVBO(); }

    public:
        unsigned int _id;      // id of vertex array
        unsigned int _idColor; // id of color array
        unsigned int _size;    // size of vertex/color array
        bool _color;           // display with colors
        bool _colorDifferent;  // is color array the same than vertex array

        void selfColor(bool c) { _color = c; }
        // VBO management
        void createVBO(const void* data, int size, GLenum usage = GL_STATIC_DRAW,
                       const void* dataColor = NULL);                                              // create a new VBO
        void deleteVBO();                                                                          // delete the VBO
        void genBuffer(unsigned int& id, const void* data, int size, GLenum target, GLenum usage); // generate VBO data
        void genBufferColor(unsigned int& idColor, const void* data, int size, GLenum target,
                            GLenum usage); // generate VBO data
        void draw();                       // draw VBO on screen
    };

public:
    // Class arguments
    OfxPointI _size;                // size of source clip
    OfxTime _time;                  // current time in sequence
    bool _isVBOBuilt;               // if VBO is not built don't draw it on overlay
    bool _isSelectionVBOBuilt;      // if color selection VBO is not built don't draw it on overlay
    bool _isSpillSelectionVBOBuilt; // if spill selection VBO is not built don't draw it on overlay

    // VBO to draw
    VBO _imgVBO;            // VBO to display on overlay (cloud point VBO)
    VBO _selectionColorVBO; // VBO to display on overlay (color selection in white)
    VBO _selectionSpillVBO; // VBO to display on overlay (spill selection in silver)

    // Data recopy
    DataVector _imgCopy;       // copy of the image needed to draw VB0
    DataVector _selectionCopy; // copy of the selection image to draw VBO
    DataVector _spillCopy;     // copy of the selection colors to draw VBO

    // Overlay data
    SelectionAverage _averageColor;  // color clip selection average
    GeodesicForm _geodesicFormColor; // geodesic form color (overlay)
    GeodesicForm _geodesicFormSpill; // geodesic form spill (overlay)

public:
    // Constructor
    CloudPointData(const OfxPointI& size, OfxTime time);

    // VBO management
    bool generateVBOData(OFX::Clip* clipSrc, const OfxPointD& renderScale, bool vboWithDiscretization,
                         int discretizationStep); // create new VBO data (fill up buffer)
    bool generateColorSelectionVBO(OFX::Clip* clipColor, const OfxPointD& renderScale, bool vboWithDiscretization,
                                   int discretizationStep); // Change color of selected pixel (color clip)
    bool generateSpillSelectionVBO(OFX::Clip* clipSpill, const OfxPointD& renderScale, bool vboWithDiscretization,
                                   int discretizationStep); // Change color of selected pixel (spill clip)
    void updateVBO();                                       // create the VBO from VBO data (draw function)

private:
    // VBO data management
    int generateAllPointsVBOData(SView srcView);                                  // generate a VBO with all of the pixels
    int generateDiscretizedVBOData(SView srcView, const int& discretizationStep); // generate a  VBO with discretization
    // selection VBO data management
    int generateAllPointsSelectionVBOData(SView srcView); // generate a VBO (and color) with all of the selected pixels
    int generateAllPointsSpillVBOData(SView srcView);     // generate a VBO (and color) with all of the selected pixels
};
}
}
}
#endif /* CLOUDPOINTDATA_HPP */
