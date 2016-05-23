#include <boost/numeric/conversion/cast.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/sqrt.hpp>
#include <terry/numeric/operations_assign.hpp>

namespace tuttle
{
namespace plugin
{
namespace idKeyer
{

template <class View>
IdKeyerProcess<View>::IdKeyerProcess(IdKeyerPlugin& instance)
    : Parent(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void IdKeyerProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    Parent::setup(args);

    _params = _plugin.getProcessParams<View>();
}

struct id_keyer_t
{
    typedef std::vector<terry::rgba32f_pixel_t> PixelList;
    PixelList _colors;
    double _tolerance;
    bool _useAlpha;

    id_keyer_t(PixelList colors, double tolerance, bool useAlpha)
        : _colors(colors)
        , _tolerance(tolerance)
        , _useAlpha(useAlpha)
    {
    }

    template <class Pixel>
    Pixel operator()(const Pixel& src) const
    {
        using namespace terry;
        typedef rgba32f_pixel_t P;

        Pixel res = src;

        bool isSelectPixel = false;

        for(PixelList::const_iterator color = _colors.begin(); color != _colors.end(); ++color)
        {
            // TUTTLE_LOG_WARNING( get_color( res, red_t()   ) << "\t" << get_color( *color, red_t()   ) );
            P min(*color);
            P max(*color);
            double minFactor = 1.0 - _tolerance;
            double maxFactor = 1.0 + _tolerance;
            numeric::pixel_multiplies_scalar_assign_t<double, P>()(minFactor, min); // min *= minFactor
            numeric::pixel_multiplies_scalar_assign_t<double, P>()(maxFactor, max); // max *= maxFactor

            if(get_color(res, red_t()) >= get_color(min, red_t()) && get_color(res, red_t()) <= get_color(max, red_t()) &&
               get_color(res, green_t()) >= get_color(min, green_t()) &&
               get_color(res, green_t()) <= get_color(max, green_t()) &&
               get_color(res, blue_t()) >= get_color(min, blue_t()) &&
               get_color(res, blue_t()) <= get_color(max, blue_t()) &&
               (!_useAlpha || (get_color(res, alpha_t()) >= get_color(min, alpha_t()) &&
                               get_color(res, alpha_t()) <= get_color(max, alpha_t()))))
            {
                isSelectPixel = true;
            }
        }

        get_color(res, alpha_t()) = isSelectPixel ? 1.0 : 0.0;
        return res;
    }
};

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void IdKeyerProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    transform_pixels_progress(src, dst, id_keyer_t(_params._colors, _params._tolerance, _params._useAlpha), *this);
}
}
}
}
