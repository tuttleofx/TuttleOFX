#include <tuttle/common/utils/global.hpp>

#define png_infopp_NULL (png_infopp) NULL
#define int_p_NULL (int*) NULL
// Should be included first to avoid setjmp.h include troubles
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

#include <terry/filter/canny.hpp>

int main(int argc, char** argv)
{
    try
    {
        using namespace boost::gil;

        boost::gil::rgb32f_image_t img;
        boost::gil::png_read_and_convert_image("data/input.png", img);
        boost::gil::rgb32f_view_t imgView(view(img));

        TUTTLE_LOG_INFO("input - x:" << imgView.width() << ", y:" << imgView.height());
        TUTTLE_LOG_INFO("input - size:" << imgView.size());

        boost::gil::gray32f_image_t imgCanny(imgView.width(), imgView.height());
        boost::gil::rgb32f_image_t tmpImgRgb(imgView.width(), imgView.height());
        boost::gil::gray32f_image_t tmpImgGray(imgView.width(), imgView.height());

        boost::timer t;
        terry::filter::canny<std::allocator>(imgView, view(tmpImgRgb), view(tmpImgGray), view(imgCanny),
                                             point2<double>(1, 1), terry::filter::convolve_option_extend_zero, 0.025, 0.1);
        TUTTLE_LOG_INFO("full canny time: " << t.elapsed());

        boost::gil::png_write_view("data/terry/output_canny_terry.png", color_converted_view<rgb8_pixel_t>(view(imgCanny)));
    }
    catch(...)
    {
        TUTTLE_LOG_ERROR("Exception ... : main de sam.");
        TUTTLE_LOG_ERROR(boost::current_exception_diagnostic_information());
    }

    return 0;
}
