#ifndef OVERLAYDATA_HPP
#define OVERLAYDATA_HPP

#include "HistogramKeyerDefinitions.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/gil/extension/color/hsl.hpp>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

typedef long Number;
typedef std::vector<Number, OfxAllocator<Number> > HistogramVector;

/*
 * structure of 7 buffers (contains histogram data)
 */
struct HistogramBufferData
{
    // step
    int _step; // nbStep (for computing and display)
    // RGB
    HistogramVector _bufferRed;   // R
    HistogramVector _bufferGreen; // G
    HistogramVector _bufferBlue;  // B
    /// HLS
    HistogramVector _bufferHue;        // H
    HistogramVector _bufferLightness;  // S
    HistogramVector _bufferSaturation; // L
    // Alpha
    HistogramVector _bufferAlpha; // alpha
};

/*
 * structure which contains selection average for each channel to display average bar
 */
struct AverageBarData
{
    // RGB
    int _averageRed;   // R
    int _averageGreen; // G
    int _averageBlue;  // B
    // HSL
    int _averageHue;        // H
    int _averageSaturation; // S
    int _averageLightness;  // L
};

/*
 *functor to compute selection histograms
 */
typedef boost::multi_array<unsigned char, 2, OfxAllocator<unsigned char> > bool_2d;

struct Pixel_compute_histograms
{
    HistogramBufferData& _data; // HistogramBufferData to fill up
    bool_2d& _imgBool;          // bool selection img (pixels)
    std::ssize_t _height;       // height of src clip
    std::ssize_t _width;        // width of src clip
    std::ssize_t _y, _x;        // position of the current pixel (functor needs to know which pixel is it)
    bool _isSelectionMode;      // do we work on all of the pixels (normal histograms) or only on selection

    Pixel_compute_histograms(bool_2d& selection, HistogramBufferData& data, const bool isSelectionMode)
        : _data(data)
        , _imgBool(selection)
        , _height(_imgBool.shape()[0])
        , _width(_imgBool.shape()[1])
        , _y(0)
        , _x(0)
        , _isSelectionMode(isSelectionMode)
    {
    }

    // basic round function
    double round(const double x) const
    {
        if(x >= 0.5)
        {
            return ceil(x);
        }
        else
        {
            return floor(x);
        }
    }

    template <typename Pixel>
    Pixel operator()(const Pixel& p)
    {
        using namespace boost::gil;
        bool ok = false;

        BOOST_ASSERT(_y >= 0);
        BOOST_ASSERT(_x >= 0);
        BOOST_ASSERT(std::ssize_t(_imgBool.shape()[0]) > _y);
        BOOST_ASSERT(std::ssize_t(_imgBool.shape()[1]) > _x);

        // int revert_y = (_height-1)-_y;

        if(_isSelectionMode == false)
            ok = true;
        else if(_imgBool[_y][_x] && _isSelectionMode)
            ok = true;

        if(ok) // if current pixel is selected
        {
            int indice;
            double val;
            hsl32f_pixel_t hsl_pix; // needed to work in HSL (entry is RGBA)
            rgba32f_pixel_t pix;

            color_convert(p, pix);       // convert input to RGBA
            color_convert(pix, hsl_pix); // convert RGBA tp HSL

            // RGBA
            for(int v = 0; v < boost::gil::num_channels<Pixel>::type::value; ++v)
            {
                val = p[v];
                if(val >= 0 && val <= 1)
                {
                    double inter = round(val * (_data._step - 1));
                    indice = inter;
                    if(v == 0)
                        _data._bufferRed.at(indice) += 1; // increments red buffer
                    else if(v == 1)
                        _data._bufferGreen.at(indice) += 1; // increments green buffer
                    else if(v == 2)
                        _data._bufferBlue.at(indice) += 1; // increments blue buffer
                    else if(v == 3)
                        _data._bufferAlpha.at(indice) += 1; // increments alpha buffer
                }
            }

            // HLS
            for(int v = 0; v < boost::gil::num_channels<hsl32f_pixel_t>::type::value; ++v)
            {
                val = hsl_pix[v];
                if(val >= 0 && val <= 1)
                {
                    double inter = round(val * (_data._step - 1));
                    indice = inter;
                    if(v == 0)
                        _data._bufferHue.at(indice) += 1; // increments hue buffer
                    else if(v == 2)
                        _data._bufferLightness.at(indice) += 1; // increments saturation buffer
                    else if(v == 1)
                        _data._bufferSaturation.at(indice) += 1; // increments lightness buffer
                }
            }
        }

        // Check pixel position
        ++_x;
        if(_x == _width)
        {
            ++_y;
            _x = 0;
        }
        return p;
    }
};

class OverlayData
{
public:
    typedef boost::gil::rgba32f_view_t SView; // declare current view type

public:
    OverlayData(const OfxPointI& size, const int nbSteps, const int nbStepsCurvesFromSelection);

    /**
     * reset selection data (button clear selection)
     */
    void clearSelection()
    {
        resetHistogramSelectionData();
        removeSelection();
        resetAverages();
    }
    void clearAll(const OfxPointI& size)
    {
        _size = size;
        resetHistogramData();
        resetHistogramSelectionData();
        removeSelection();
        resetAverages();
    }

    /**
     * Image size checker
     * @warning HACK changeClip method doesn't work in nuke when time of source clip is changed so we have to check size of
     * imgBool all the time
     */
    bool isImageSizeModified(const OfxPointI& size) const;

    /**
     * Histogram computing
     */
    void computeFullData(OFX::Clip* clipSrc, const OfxTime time, const OfxPointD& renderScale,
                         const bool selectionOnly = false); // compute full data (average/selection/histograms)
    void computeCurveFromSelectionData(OFX::Clip* clipSrc, const OfxTime time,
                                       const OfxPointD& renderScale); // compute only selection to curve data
    void setNbStep(const std::size_t nbStep) { _vNbStep = nbStep; }

    /**
     * Current time checker
 */
    bool isCurrentTimeModified(const OfxTime time) const;

    /**
     * HistoramData management
     */
    void resetCurvesFromSelectionData(); // reset curves from selection data

private:
    /*Histogram management*/
    void computeHistogramBufferData(HistogramBufferData& data, SView& srcView, const OfxTime time,
                                    const bool isSelection = false);       // compute a HisogramBufferData
    void correctHistogramBufferData(HistogramBufferData& toCorrect) const; // correct a complete HistogramBufferData
    void resetHistogramBufferData(HistogramBufferData& toReset) const;     // reset a complete HistogramBufferData

    /*Average management*/
    void computeAverages(); // compute average of each channel

    /*Reset data*/
    void resetHistogramData(); // reset data (if size change for example)

    void resetHistogramSelectionData(); // reset selection data
    void resetAverages();               // rest all of the averages
    void removeSelection();

    void correctVector(HistogramVector& v) const;                             // correct a specific channel
    void resetVectortoZero(HistogramVector& v, const std::size_t size) const; // reset a specific channel buffer
    int computeAnAverage(const HistogramVector& selection_v) const;           // compute average of a specific channel

public:
    ///@todo accessors
    HistogramBufferData _data;               // histogram data
    HistogramBufferData _selectionData;      // selection histogram data
    HistogramBufferData _curveFromSelection; // curve from selection histogram data

    AverageBarData _averageData;            // average bar data used to display average bars
    bool_2d _imgBool;                       // unsigned char 2D (use for display texture on screen)
    OfxTime _currentTime;                   // time of the current frame
    std::size_t _vNbStep;                   // nbStep for buffers
    std::size_t _vNbStepCurveFromSelection; // nbStep for curve to selection buffers
    bool _isComputing;

    bool _isDataInvalid;

private:
    OfxPointI _size; // source clip size
};
}
}
}

#endif
