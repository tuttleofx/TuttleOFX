#include "PrintAlgorithm.hpp"
#include "PrintPlugin.hpp"

#include <terry/channel.hpp>
#include <terry/algorithm/pixel_by_channel.hpp>

#include <cstdio>

namespace tuttle
{
namespace plugin
{
namespace print
{

struct CacaImage
{
    char* pixels;
    unsigned int w, h;
    caca_dither_t* dither;
};

template <class SView>
CacaImage load_cacaimage_from_view(const SView sView)
{
    typedef typename boost::gil::channel_mapping_type<SView>::type Channel;
    typedef typename terry::channel_base_type<Channel>::type ChannelBaseType;

    CacaImage im;
    unsigned int bpp, rmask, gmask, bmask, amask;

    im.pixels = (char*)boost::gil::interleaved_view_get_raw_data(sView);
    im.w = sView.width();
    im.h = sView.height();
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0x00000000;
    bpp = boost::gil::num_channels<SView>::value * sizeof(ChannelBaseType) * 8;

    // Create the libcaca dither
    im.dither = caca_create_dither(bpp, im.w, im.h, sView.pixels().row_size(), rmask, gmask, bmask, amask);

    if(!im.dither)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::dev("Unable to load buffer."));
    }
    return im;
}

template <>
CacaImage load_cacaimage_from_view<boost::gil::gray8_view_t>(const boost::gil::gray8_view_t sView)
{
    typedef boost::gil::channel_mapping_type<boost::gil::gray8_view_t>::type Channel;
    typedef terry::channel_base_type<Channel>::type ChannelBaseType;

    CacaImage im;
    unsigned int bpp, rmask, gmask, bmask, amask;

    im.pixels = (char*)boost::gil::interleaved_view_get_raw_data(sView);
    im.w = sView.width();
    im.h = sView.height();
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x00000000;
    bpp = boost::gil::num_channels<boost::gil::gray8_view_t>::value * sizeof(ChannelBaseType) * 8;

    // Create the libcaca dither
    im.dither = caca_create_dither(bpp, im.w, im.h, sView.pixels().row_size(), rmask, gmask, bmask, amask);

    if(!im.dither)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::dev("Unable to load buffer."));
    }
    return im;
}

template <typename Channel>
struct channel_cout_t : public std::unary_function<Channel, Channel>
{
    GIL_FORCEINLINE
    Channel operator()(typename boost::gil::channel_traits<Channel>::const_reference ch) const
    {
        std::cout << ch << " ";
        return ch;
    }
};

template <template <class> class Func>
struct call_pixel_by_channel_t
{
    template <typename Pixel>
    GIL_FORCEINLINE Pixel operator()(const Pixel& v) const
    {
        static_for_each(v, Func<typename boost::gil::channel_type<Pixel>::type>());
        return v;
    }
};

template <class View>
PrintProcess<View>::PrintProcess(PrintPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationFromTopToBottom)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
PrintProcess<View>::~PrintProcess()
{
}

template <class View>
void PrintProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void PrintProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    // OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);

    View src = this->_srcView;
    View dst = this->_dstView;

    OfxRectI region = procWindowSrc;

    copy_pixels(src, dst);

    switch(_params._mode)
    {
        case eParamModePixel:
        {
            call_pixel_by_channel_t<channel_cout_t>()(src(_params._pixel));
            std::cout << std::endl;
            break;
        }
        case eParamModeRegion:
        {
            src = subimage_view(this->_srcView, _params._region.x1, _params._region.y1,
                                _params._region.x2 - _params._region.x1, _params._region.y2 - _params._region.y1);
            region = _params._region;
            // continue with #src as a part of the image...
        }
        case eParamModeImage:
        {
            switch(_params._output)
            {
                case eParamOutputNumeric:
                {
                    std::cout << std::fixed;
                    std::cout << std::setprecision(2);
                    call_pixel_by_channel_t<channel_cout_t> proc;
                    for(int y = region.y1; y < region.y2; ++y)
                    {
                        std::cout << "| ";
                        typename View::x_iterator src_it = this->_srcView.x_at(region.x1, y);
                        for(int x = region.x1; x < region.x2; ++x, ++src_it)
                        {
                            proc(*src_it);
                            std::cout << " | ";
                        }
                        std::cout << "\n";
                        if(this->progressForward(region.x2 - region.x1))
                            return;
                    }
                    break;
                }
                case eParamOutputAscii:
                {
                    // temporary gray buffer to compute the char values.
                    gray8_image_t gImgGray(src.dimensions());
                    gray8_view_t gViewGray(view(gImgGray));
                    rgb8_image_t gImg(src.dimensions());
                    rgb8_view_t gView(view(gImg));

                    if(_params._flip)
                    {
                        src = flipped_up_down_view(src);
                    }

                    switch(_params._colorMode)
                    {
                        case eParamColorMono:
                            copy_and_convert_pixels(src, gViewGray);
                            break;
                        case eParamColorGray:
                            copy_and_convert_pixels(src, gViewGray);
                            break;
                        default:
                            copy_and_convert_pixels(src, gView);
                            break;
                    }
                    // libcaca context
                    caca_canvas_t* cv = NULL;

                    try
                    {
                        struct CacaImage cacaImg;
                        unsigned int cols = 0, lines = 0, font_width = 6, font_height = 10;
                        char* dither = NULL;
                        float gamma = -1, brightness = -1, contrast = -1;

                        cv = caca_create_canvas(0, 0);
                        if(!cv)
                        {
                            BOOST_THROW_EXCEPTION(exception::Failed()
                                                  << exception::user("Print: unable to initialise libcaca"));
                        }
                        switch(_params._colorMode)
                        {
                            case eParamColorMono:
                                cacaImg = load_cacaimage_from_view(gViewGray);
                                break;
                            case eParamColorGray:
                                cacaImg = load_cacaimage_from_view(gViewGray);
                                break;
                            default:
                                cacaImg = load_cacaimage_from_view(gView);
                                break;
                        }

                        /*
                         *  - \c "mono": use light gray on a black background.
                         *  - \c "gray": use white and two shades of gray on a black background.
                         *  - \c "8": use the 8 ANSI colours on a black background.
                         *  - \c "16": use the 16 ANSI colours on a black background.
                         *  - \c "fullgray": use black, white and two shades of gray for both the
                         *    characters and the background.
                         *  - \c "full8": use the 8 ANSI colours for both the characters and the
                         *    background.
                         *  - \c "full16" or \c "default": use the 16 ANSI colours for both the
                         *    characters and the background. This is the default value.
                         */
                        std::string colorMode;
                        switch(_params._colorMode)
                        {
                            case eParamColorMono:
                                colorMode = "mono";
                                break;
                            case eParamColorGray:
                                colorMode = "gray";
                                break;
                            case eParamColor8:
                                colorMode = "8";
                                break;
                            case eParamColor16:
                                colorMode = "16";
                                break;
                            case eParamColorfullgray:
                                colorMode = "fullgray";
                                break;
                            case eParamColorfull8:
                                colorMode = "full8";
                                break;
                            case eParamColorfull16:
                                colorMode = "full16";
                                break;
                        }
                        caca_set_dither_color(cacaImg.dither, colorMode.c_str());

                        if(!cols && !lines)
                        {
                            cols = _params._cols + 1;
                            lines = cols * cacaImg.h * font_width / cacaImg.w / font_height;
                        }
                        else if(cols && !lines)
                        {
                            lines = cols * cacaImg.h * font_width / cacaImg.w / font_height;
                        }
                        else if(!cols && lines)
                        {
                            cols = lines * cacaImg.w * font_height / cacaImg.h / font_width;
                        }
                        // TUTTLE_LOG_TRACE( "output : " << cols << " x " << lines );
                        caca_set_canvas_size(cv, cols, lines);

                        caca_clear_canvas(cv);

                        if(caca_set_dither_algorithm(cacaImg.dither, dither ? dither : "fstein"))
                        {
                            BOOST_THROW_EXCEPTION(exception::Unknown()
                                                  << exception::dev() + "Can't dither image with algorithm " + dither);
                        }

                        if(brightness != -1)
                            caca_set_dither_brightness(cacaImg.dither, brightness);
                        if(contrast != -1)
                            caca_set_dither_contrast(cacaImg.dither, contrast);
                        if(gamma != -1)
                            caca_set_dither_gamma(cacaImg.dither, gamma);

                        caca_dither_bitmap(cv, 0, 0, cols, lines, cacaImg.dither, cacaImg.pixels);

                        caca_free_dither(cacaImg.dither);

                        if(_params._openGlViewer == true)
                        {
                            // show result in a new window
                            caca_display_t* dp;
                            caca_event_t ev;
                            dp = caca_create_display(cv);

                            caca_set_display_title(dp, "Rendering image in ASCII Art");

                            caca_refresh_display(dp);
                            caca_get_event(dp, CACA_EVENT_KEY_PRESS | CACA_EVENT_QUIT | CACA_EVENT_MOUSE_RELEASE, &ev, -1);
                            caca_free_display(dp);

                            caca_free_canvas(cv);
                        }
                        else
                        {
                            // clear the sreen
                            std::cout << std::string(100, '\n');

                            size_t len;
                            void* output;

                            output = caca_export_canvas_to_memory(cv, "ansi", &len);
                            fwrite(output, len, 1, stdout);
                            free(output);

                            caca_free_canvas(cv);
                        }
                    }
                    catch(...)
                    {
                        TUTTLE_LOG_CURRENT_EXCEPTION;
                        if(cv != NULL)
                            caca_free_canvas(cv);
                    }
                    break;
                }
            }
            break;
        }
    }
}
}
}
}
