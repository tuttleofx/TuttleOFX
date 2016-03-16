#ifndef _PNG_ADDS_HPP
#define _PNG_ADDS_HPP

#include <stdio.h>
#include <string>

extern "C" {
#include "png.h"
}

#include <boost/static_assert.hpp>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/extension/io/io_error.hpp>
#include <boost/gil/extension/io/png_io_private.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

class png_reader_info : public file_mgr
{
protected:
    png_structp _png_ptr;
    png_infop _info_ptr;
    int bit_depth, color_type, interlace_type;

public:
    png_reader_info(const std::string& filename)
        : file_mgr(filename.c_str(), "rb")
    {
        init();
    }

    void init()
    {
        char buf[PNG_BYTES_TO_CHECK];

        // read in some of the signature bytes
        io_error_if(fread(buf, 1, PNG_BYTES_TO_CHECK, get()) != detail::PNG_BYTES_TO_CHECK,
                    "png_check_validity: fail to read file");
        // compare the first PNG_BYTES_TO_CHECK bytes of the signature.
        io_error_if(png_sig_cmp((png_bytep)buf, (png_size_t)0, detail::PNG_BYTES_TO_CHECK) != 0,
                    "png_check_validity: invalid png file");

        _png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        io_error_if(_png_ptr == NULL, "png_get_file_size: fail to call png_create_write_struct()");
        // allocate/initialize the image information data
        _info_ptr = png_create_info_struct(_png_ptr);
        if(_info_ptr == NULL)
        {
            png_destroy_read_struct(&_png_ptr, png_infopp_NULL, png_infopp_NULL);
            io_error("png_get_file_size: fail to call png_create_info_struct()");
        }
        if(setjmp(png_jmpbuf(_png_ptr)))
        {
            // free all of the memory associated with the png_ptr and info_ptr
            png_destroy_read_struct(&_png_ptr, &_info_ptr, png_infopp_NULL);
            io_error("png_get_file_size: fail to call setjmp()");
        }
        png_init_io(_png_ptr, get());
        png_set_sig_bytes(_png_ptr, PNG_BYTES_TO_CHECK);
        png_read_info(_png_ptr, _info_ptr);
        if(little_endian() && png_get_bit_depth(_png_ptr, _info_ptr) > 8)
            png_set_swap(_png_ptr);
    }

    virtual ~png_reader_info() { png_destroy_read_struct(&_png_ptr, &_info_ptr, png_infopp_NULL); }

    void read_header()
    {
        png_uint_32 width, height;

        png_get_IHDR(_png_ptr, _info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
    }

    int get_bit_depth() { return bit_depth; }
    int get_color_type() { return color_type; }
    int get_interlace_type() { return interlace_type; }
};
}

/// \ingroup PNG_IO
/// \brief Returns the precision of the PNG file at the specified location.
/// Throws std::ios_base::failure if the location does not correspond to a valid PNG file
inline size_t png_read_precision(const std::string& filename)
{
    detail::png_reader_info m(filename);

    m.read_header();
    return m.get_bit_depth();
}

/// \ingroup PNG_IO
/// \brief describes which color/alpha channels are present.
///  PNG_COLOR_TYPE_GRAY        (bit depths 1, 2, 4, 8, 16)
///  PNG_COLOR_TYPE_GRAY_ALPHA  (bit depths 8, 16)
///  PNG_COLOR_TYPE_PALETTE     (bit depths 1, 2, 4, 8)
///  PNG_COLOR_TYPE_RGB         (bit_depths 8, 16)
///  PNG_COLOR_TYPE_RGB_ALPHA   (bit_depths 8, 16)
///  PNG_COLOR_MASK_PALETTE
///  PNG_COLOR_MASK_COLOR
///  PNG_COLOR_MASK_ALPHA
inline size_t png_read_color_type(const std::string& filename)
{
    detail::png_reader_info m(filename);

    m.read_header();
    return m.get_color_type();
}
}
}

#endif /* _PNG_ADDS_HPP */
