#include "ColorTransferProcess.hpp"
#include "ColorTransferPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/param/gilColor.hpp>

#include <terry/color/transfer.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/sqrt.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/globals.hpp>

#include <boost/units/pow.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

#include <boost/numeric/ublas/vector.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransfer
{

using namespace terry;
using namespace terry::color;
using namespace terry::color::transfer;
using namespace terry::numeric;

template <typename Pixel, typename CPixel>
CPixel getPixel(const Pixel& p, const EColorspace eColorspace)
{
    switch(eColorspace)
    {
        case eColorspaceNone:
        {
            CPixel cp;
            pixel_assigns_t<Pixel, CPixel>()(p, cp);
            return cp;
        }
        case eColorspaceLMS:
        {
            return pixel_rgb_to_lms_t<Pixel, CPixel>()(p);
        }
        case eColorspaceLab:
        {
            return pixel_rgb_to_lab_t<Pixel, CPixel>()(p);
        }
    }
    BOOST_ASSERT(false);
    CPixel cp;
    pixel_assigns_t<Pixel, CPixel>()(p, cp);
    return cp;
}

template <typename Pixel, typename CPixel>
CPixel setPixel(const Pixel& p, const EColorspace eColorspace)
{
    switch(eColorspace)
    {
        case eColorspaceNone:
        {
            CPixel cp;
            pixel_assigns_t<CPixel, Pixel>()(p, cp);
            return cp;
        }
        case eColorspaceLMS:
        {
            return pixel_lms_to_rgb_t<Pixel, CPixel>()(p);
        }
        case eColorspaceLab:
        {
            return pixel_lab_to_rgb_t<Pixel, CPixel>()(p);
        }
    }
    BOOST_ASSERT(false);
    CPixel cp;
    pixel_assigns_t<Pixel, CPixel>()(p, cp);
    return cp;
}

template <class View>
struct ColorParams
{
    typedef typename View::value_type Pixel;
    Pixel _srcAverage, _dstAverage, _deviationRatio;
    EColorspace _eColorspace;

    ColorParams(const Pixel& srcAverage, const Pixel& dstAverage, const Pixel& deviationRatio, const EColorspace eColorspace)
    {
        pixel_assigns_t<Pixel, Pixel>()(deviationRatio, _deviationRatio);
        pixel_assigns_t<Pixel, Pixel>()(srcAverage, _srcAverage);
        pixel_assigns_t<Pixel, Pixel>()(dstAverage, _dstAverage);
        _eColorspace = eColorspace;

        //		TUTTLE_LOG_VAR( TUTTLE_INFO, get_color( _deviationRatio, red_t() ) );
        //		TUTTLE_LOG_VAR( TUTTLE_INFO, get_color( _srcAverage, red_t() ) );
        //		TUTTLE_LOG_VAR( TUTTLE_INFO, get_color( _dstAverage, red_t() ) );
    }

    Pixel operator()(const Pixel& p) const
    {
        //		Pixel p2; pixel_assigns_t<Pixel, Pixel > ( )( p, p2 );
        // RGB to LAB
        Pixel p2 = getPixel<Pixel, Pixel>(p, _eColorspace);

        pixel_minus_assign_t<Pixel, Pixel>()(_srcAverage, p2);
        p2 = pixel_multiplies_t<Pixel, Pixel, Pixel>()(_deviationRatio, p2);
        pixel_plus_assign_t<Pixel, Pixel>()(_dstAverage, p2);

        // LAB to RGB
        return setPixel<Pixel, Pixel>(p2, _eColorspace);
    }
};

template <class View>
ColorTransferProcess<View>::ColorTransferProcess(ColorTransferPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ColorTransferProcess<View>::computeAverage(const View& image, Pixel& average, Pixel& deviation,
                                                const EColorspace& eColorspace)
{
    typedef typename color_space_type<View>::type Colorspace;
    typedef pixel<boost::gil::bits64f, layout<Colorspace> > CPixel;
    CPixel sumAverage, sumDeviation;
    pixel_zeros_t<CPixel>()(sumAverage);
    pixel_zeros_t<CPixel>()(sumDeviation);
    //	const std::size_t nbPixels = image.width( ) * image.height( );

    // Average
    CPixel cAverage;
    pixel_zeros_t<CPixel>()(cAverage);
    for(std::ssize_t y = 0; y < image.height(); ++y)
    {
        CPixel sumAverageLine;
        pixel_zeros_t<CPixel>()(sumAverageLine);
        typename View::x_iterator src_it = image.x_at(0, y);
        for(std::ssize_t x = 0; x < image.width(); ++x, ++src_it)
        {
            //			CPixel pix; pixel_assigns_t<Pixel, CPixel>()( * src_it, pix );
            CPixel pix = getPixel<Pixel, CPixel>(*src_it, eColorspace);
            pixel_plus_assign_t<CPixel, CPixel>()(pix, sumAverageLine);
        }
        pixel_divides_scalar_assign_t<double, CPixel>()(image.width(), sumAverageLine);
        pixel_plus_assign_t<CPixel>()(sumAverageLine, cAverage);
    }
    pixel_divides_scalar_assign_t<double, CPixel>()(image.height(), cAverage);
    pixel_assigns_t<CPixel, Pixel>()(cAverage, average);
    //	average = pixel_divides_scalar_t<CPixel, double, Pixel>()( sumAverage, nbPixels );

    CPixel cDeviation;
    pixel_zeros_t<CPixel>()(cDeviation);
    // Standard deviation
    for(std::ssize_t y = 0; y < image.height(); ++y)
    {
        CPixel sumDeviationLine;
        pixel_zeros_t<CPixel>()(sumDeviationLine);
        typename View::x_iterator src_it = image.x_at(0, y);
        for(std::ssize_t x = 0; x < image.width(); ++x, ++src_it)
        {
            //			CPixel pix; pixel_assigns_t<Pixel, CPixel>()( *src_it, pix );
            CPixel pix = getPixel<Pixel, CPixel>(*src_it, eColorspace);
            pixel_minus_assign_t<CPixel>()(cAverage, pix);
            pix = pixel_pow_t<CPixel, 2>()(pix);
            pixel_plus_assign_t<CPixel>()(pix, sumDeviationLine);
        }
        pixel_plus_assign_t<CPixel>()(pixel_divides_scalar_t<CPixel, double>()(sumDeviationLine, image.width()), cDeviation);
    }
    pixel_divides_scalar_assign_t<double, CPixel>()(image.height(), cDeviation);
    deviation = pixel_sqrt_t<CPixel, Pixel>()(cDeviation);
}

template <class View>
void ColorTransferProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);           // Call parent class setup
    _params = _plugin.getProcessParams(args.renderScale); // Retrieve plugin parameters

    // srcRef initialization
    if(_plugin._clipSrcRef->isConnected())
    {
        this->_srcRef.reset(_plugin._clipSrcRef->fetchImage(args.time));
        if(!this->_srcRef.get())
        {
            BOOST_THROW_EXCEPTION(exception::ImageNotReady());
        }
        if(this->_srcRef->getRowDistanceBytes() == 0)
        {
            BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        }

        if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
        {
            // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
            _srcRefPixelRod = _plugin._clipSrcRef->getPixelRod(args.time, args.renderScale);
        }
        else
        {
            _srcRefPixelRod = _srcRef->getRegionOfDefinition();
        }
        this->_srcRefView = this->getView(this->_srcRef.get(), _srcRefPixelRod);
    }
    else
    {
        this->_srcRefPixelRod = this->_srcPixelRod;
        this->_srcRefView = this->_srcView;
    }

    // dstRef initialization
    this->_dstRef.reset(_plugin._clipDstRef->fetchImage(args.time));
    if(!this->_dstRef.get())
    {
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    }
    if(this->_dstRef->getRowDistanceBytes() == 0)
    {
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
    }

    if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
    {
        // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
        _dstRefPixelRod = _plugin._clipDstRef->getPixelRod(args.time, args.renderScale);
    }
    else
    {
        _dstRefPixelRod = _dstRef->getRegionOfDefinition();
    }
    this->_dstRefView = this->getView(this->_dstRef.get(), _dstRefPixelRod);

    // analyse srcRef and dstRef
    Pixel srcRefDeviation, dstRefDeviation;
    computeAverage(this->_srcRefView, _srcRefAverage, srcRefDeviation, _params._colorspace);
    computeAverage(this->_dstRefView, _dstRefAverage, dstRefDeviation, _params._colorspace);
    // TUTTLE_LOG_VAR4( TUTTLE_INFO, _srcRefAverage[0], _srcRefDeviation[0], _dstRefAverage[0], _dstRefDeviation[0]);

    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(dstRefDeviation, red_t()));
    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(dstRefDeviation, green_t()));
    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(dstRefDeviation, blue_t()));
    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(srcRefDeviation, red_t()));
    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(srcRefDeviation, green_t()));
    TUTTLE_LOG_VAR(TUTTLE_INFO, get_color(srcRefDeviation, blue_t()));

    // now analyse the differences
    pixel_zeros_t<Pixel>()(_deviationRatio);
    _deviationRatio = pixel_divides_t<Pixel, Pixel, Pixel>()(dstRefDeviation, srcRefDeviation);

    // modify the values with coefficient parameters
    if(_params._averageCoef != 1.0)
    {
        // recompute the dstAverage weighted by the averageCoef
        // - scale the vector from srcAverage to dstAverage
        Pixel vecAverage = pixel_minus_t<Pixel, Pixel, Pixel>()(_dstRefAverage, _srcRefAverage);
        pixel_multiplies_scalar_assign_t<double, Pixel>()(_params._averageCoef, vecAverage);
        // - apply this new vector to srcAverage
        _dstRefAverage = pixel_plus_t<Pixel, Pixel, Pixel>()(_srcRefAverage, vecAverage);
    }
    if(_params._dynamicCoef != 1.0)
    {
        // scale the deviationRatio, centered around 1, with the dynamicCoef
        Pixel ones;
        pixel_ones_t<Pixel>()(ones);
        pixel_minus_assign_t<Pixel, Pixel>()(ones, _deviationRatio);
        pixel_multiplies_scalar_assign_t<double, Pixel>()(_params._dynamicCoef, _deviationRatio);
        pixel_plus_assign_t<Pixel, Pixel>()(ones, _deviationRatio);
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorTransferProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    // fill dst: modify src using analyse of srcRef and dstRef differences
    terry::algorithm::transform_pixels_progress(
        src, dst, ColorParams<View>(_srcRefAverage, _dstRefAverage, _deviationRatio, _params._colorspace), *this);
}
}
}
}
