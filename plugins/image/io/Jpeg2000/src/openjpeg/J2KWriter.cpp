#include "J2KWriter.hpp"
#include "J2KCommon.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <terry/typedefs.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <vector>

namespace tuttle
{
namespace io
{

namespace fs = boost::filesystem;

J2KWriter::J2KWriter()
    : _width(0)
    , _height(0)
    , _components(0)
    , _precision(0)
    , _cinemaMode(OFF)
    , _cio(NULL)
{
    memset(&_openjpeg, 0, sizeof(OpenJpegStuffs));
}

J2KWriter::~J2KWriter()
{
    close();
}

void J2KWriter::open(const std::string& filename, const std::size_t w, const std::size_t h, const std::size_t nc,
                     const std::size_t dprecision)
{
    close();
    std::vector<opj_image_cmptparm_t> cmptparm(nc);
    OPJ_COLOR_SPACE color_space;
    std::size_t j;
    int subsampling_dx, subsampling_dy;

    /** Routines d'ouverture du fichier **/
    // configure the event callbacks (not required)
    // setting of each callback is optionnal
    memset(&_openjpeg, 0, sizeof(OpenJpegStuffs));

    /* set encoding parameters to default values */
    opj_set_default_encoder_parameters(&_openjpeg.parameters);
    _openjpeg.parameters.cod_format = J2K_CFMT;
    _openjpeg.parameters.cp_cinema = _cinemaMode;

    switch(_cinemaMode)
    {
        case OFF:
            _openjpeg.parameters.cp_rsiz = STD_RSIZ;
            _openjpeg.parameters.irreversible = _lossless ? 0 : 1;
            break;
        case CINEMA2K_24:
        case CINEMA2K_48:
            _openjpeg.parameters.cp_rsiz = CINEMA2K;
            break;
        case CINEMA4K_24:
            _openjpeg.parameters.cp_rsiz = CINEMA4K;
            break;
    }

    //////////////////////////////////////////////////////////////////////////
    // if no rate entered, lossless by default
    if(_openjpeg.parameters.tcp_numlayers == 0)
    {
        _openjpeg.parameters.tcp_rates[0] = 0; /* MOD antonin : losslessbug */
        _openjpeg.parameters.tcp_numlayers++;
        _openjpeg.parameters.cp_disto_alloc = 1;
    }
    if(_cinemaMode)
    {
        cinemaSetupParameters();
    }

    // Create comment for codestream
    if(_openjpeg.parameters.cp_comment == NULL)
    {
        const char comment[] = "TuttleOFX";
        const std::size_t clen = strlen(comment);
        const char* version = ""; // opj_version();
        _openjpeg.parameters.cp_comment = (char*)malloc(clen + strlen(version) + 1);
        sprintf(_openjpeg.parameters.cp_comment, "%s%s", comment, version);
    }

    //////////////////////////////////////////////////////////////////////////
    color_space = CLRSPC_UNKNOWN; // CLRSPC_SRGB;
    // initialize image components
    memset(&cmptparm[0], 0, nc * sizeof(opj_image_cmptparm_t));
    subsampling_dx = _openjpeg.parameters.subsampling_dx;
    subsampling_dy = _openjpeg.parameters.subsampling_dy;
    for(j = 0; j < nc; j++)
    {
        if(_openjpeg.parameters.cp_cinema)
        {
            cmptparm[j].prec = 12;
            cmptparm[j].bpp = 12;
        }
        else
        {
            cmptparm[j].prec = dprecision;
            cmptparm[j].bpp = dprecision;
        }
        cmptparm[j].sgnd = 0;
        cmptparm[j].dx = subsampling_dx;
        cmptparm[j].dy = subsampling_dy;
        cmptparm[j].w = w;
        cmptparm[j].h = h;
    }

    /* create the image*/
    _openjpeg.image = opj_image_create(nc, &cmptparm[0], color_space);

    _openjpeg.image->x0 = _openjpeg.parameters.image_offset_x0;
    _openjpeg.image->y0 = _openjpeg.parameters.image_offset_y0;
    _openjpeg.image->x1 =
        !_openjpeg.image->x0 ? (w - 1) * subsampling_dx + 1 : _openjpeg.image->x0 + (w - 1) * subsampling_dx + 1;
    _openjpeg.image->y1 =
        !_openjpeg.image->y0 ? (h - 1) * subsampling_dy + 1 : _openjpeg.image->y0 + (h - 1) * subsampling_dy + 1;

    /* Decide if MCT should be used */
    _openjpeg.parameters.tcp_mct = _openjpeg.image->numcomps == 3 ? 1 : 0;

    if(_openjpeg.parameters.cp_cinema != OFF)
    {
        cinemaSetupEncoder();
    }

    _outFile.open(filename, std::ios_base::out | std::ios_base::binary);
    if(!_outFile.good())
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Unable to open output file.")
                                                << exception::filename(filename));
    }
    _components = nc;
    _width = w;
    _height = h;
}

void J2KWriter::encode(const uint8_t* data, const std::size_t sprecision)
{
    using namespace boost::gil;
    OPJ_CODEC_FORMAT fmt;
    switch(_openjpeg.parameters.cod_format)
    {
        case J2K_CFMT:
        {
            fmt = CODEC_J2K;
            break;
        }
        default:
        {
            BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Unrecognized output J2K format \"" +
                                                            _openjpeg.parameters.cod_format + "\".");
        }
    }

    // Get a J2K compressor handle
    opj_cinfo_t* cinfo = opj_create_compress(fmt);
    // Catch events using our callbacks and give a local context
    opj_set_event_mgr((opj_common_ptr)cinfo, &_openjpeg.event_mgr, stderr);
    // Setup the encoder parameters using the current image and user parameters
    opj_setup_encoder(cinfo, &_openjpeg.parameters, _openjpeg.image);
    // Open a byte stream for writing
    // Allocate memory for all tiles
    _cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);

    int j = _width * _height;
    // Check cinema mode off/on
    std::vector<int*> ch32(_components);
    switch(sprecision)
    {
        case 8:
        {
            const uint8_t* sdata = (uint8_t*)data;
            for(std::size_t c = 0; c < _components; ++c)
            {
                ch32[c] = &_openjpeg.image->comps[c].data[0];
            }
            if(_cinemaMode != OFF)
            {
                // DCI is always 12 bits encoded
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        gray8_pixel_t sp(*sdata++);
                        gray12_pixel_t dp;
                        color_convert(sp, dp);
                        *ch32[c]++ = get_color(dp, gray_color_t());
                    }
                }
            }
            else
            {
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        *ch32[c]++ = *sdata++;
                    }
                }
            }
            break;
        }
        case 12:
        {
            const uint16_t* sdata = (uint16_t*)data;
            for(std::size_t c = 0; c < _components; ++c)
            {
                ch32[c] = &_openjpeg.image->comps[c].data[0];
            }
            while(j--)
            {
                for(std::size_t c = 0; c < _components; ++c)
                {
                    *(ch32[c]++) = *sdata++;
                }
            }
            break;
        }
        case 16:
        {
            const uint16_t* sdata = (uint16_t*)data;
            for(std::size_t c = 0; c < _components; ++c)
            {
                ch32[c] = &_openjpeg.image->comps[c].data[0];
            }
            if(_cinemaMode != OFF)
            {
                // DCI is always 12 bits encoded
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        gray16_pixel_t sp(*sdata++ & 0xFFFF);
                        gray12_pixel_t dp;
                        color_convert(sp, dp);
                        *(ch32[c]++) = get_color(dp, gray_color_t());
                    }
                }
            }
            else
            {
                // Encode 12 bits
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        *(ch32[c]++) = *sdata++;
                    }
                }
            }
            break;
        }
        case 32:
        {
            const uint32_t* sdata = (uint32_t*)data;
            for(std::size_t c = 0; c < _components; ++c)
            {
                ch32[c] = &_openjpeg.image->comps[c].data[0];
            }
            if(_cinemaMode != OFF)
            {
                // DCI is always 12 bits
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        gray32_pixel_t sp(*sdata++);
                        gray12_pixel_t dp;
                        color_convert(sp, dp);
                        *(ch32[c]++) = get_color(dp, gray_color_t());
                    }
                }
            }
            else
            {
                while(j--)
                {
                    for(std::size_t c = 0; c < _components; ++c)
                    {
                        *(ch32[c]++) = *sdata++;
                    }
                }
            }
            break;
        }
    }

    // Encode the image
    if(!opj_encode(cinfo, _cio, _openjpeg.image, NULL))
    {
        opj_cio_close(_cio);
        _cio = NULL;
        BOOST_THROW_EXCEPTION(exception::Failed() << exception::user("Failed to encode image!"));
    }

    // free remaining compression structures
    opj_destroy_compress(cinfo);
}

void J2KWriter::cinemaSetupParameters()
{
    _openjpeg.parameters.tile_size_on = false;
    _openjpeg.parameters.cp_tdx = 1;
    _openjpeg.parameters.cp_tdy = 1;

    // Tile part
    _openjpeg.parameters.tp_flag = 'C';
    _openjpeg.parameters.tp_on = 1;

    // Tile and Image shall be at (0,0)
    _openjpeg.parameters.cp_tx0 = 0;
    _openjpeg.parameters.cp_ty0 = 0;
    _openjpeg.parameters.image_offset_x0 = 0;
    _openjpeg.parameters.image_offset_y0 = 0;

    // Codeblock size= 32*32
    _openjpeg.parameters.cblockw_init = 32;
    _openjpeg.parameters.cblockh_init = 32;
    _openjpeg.parameters.csty |= 0x01;

    // The progression order shall be CPRL
    _openjpeg.parameters.prog_order = CPRL;

    // No ROI
    _openjpeg.parameters.roi_compno = -1;
    _openjpeg.parameters.subsampling_dx = 1;
    _openjpeg.parameters.subsampling_dy = 1;

    // 9-7 transform (instead of 5-3 reversible)
    _openjpeg.parameters.irreversible = 1;
}

void J2KWriter::cinemaSetupEncoder()
{
    switch(_openjpeg.parameters.cp_cinema)
    {
        case CINEMA2K_24:
        case CINEMA2K_48:
        {
            if(_openjpeg.parameters.numresolution > 6)
            {
                _openjpeg.parameters.numresolution = 6;
            }
            if(_openjpeg.image->comps[0].w != 2048 && _openjpeg.image->comps[0].h != 1080)
            {
                _openjpeg.parameters.cp_rsiz = STD_RSIZ;
                BOOST_THROW_EXCEPTION(exception::Logic()
                                      << exception::user() + "Image dimensions " + _openjpeg.image->comps[0].w + " x " +
                                             _openjpeg.image->comps[0].h + " is not 2K compliant.\n" +
                                             "JPEG Digital Cinema Profile-3 (2K profile) compliance requires that at least "
                                             "one of coordinates match 2048 x 1080");
            }
            break;
        }
        case CINEMA4K_24:
        {
            if(_openjpeg.parameters.numresolution < 1)
            {
                _openjpeg.parameters.numresolution = 1;
            }
            else if(_openjpeg.parameters.numresolution > 7)
            {
                _openjpeg.parameters.numresolution = 7;
            }
            if(_openjpeg.image->comps[0].w != 4096 && _openjpeg.image->comps[0].h != 2160)
            {
                _openjpeg.parameters.cp_rsiz = STD_RSIZ;
                BOOST_THROW_EXCEPTION(exception::Logic()
                                      << exception::user() + "Image dimensions " + _openjpeg.image->comps[0].w + " x " +
                                             _openjpeg.image->comps[0].h + " is not 4K compliant.\n" +
                                             "JPEG Digital Cinema Profile-3 (4K profile) compliance requires that at least "
                                             "one of coordinates match 4096 x 2160");
            }
            _openjpeg.parameters.numpocs = initialize4Kpocs(_openjpeg.parameters.POC, _openjpeg.parameters.numresolution);
            break;
        }
        default:
            break;
    }

    switch(_openjpeg.parameters.cp_cinema)
    {
        case CINEMA2K_24:
        case CINEMA4K_24:
        {
            for(int i = 0; i < _openjpeg.parameters.tcp_numlayers; ++i)
            {
                _openjpeg.parameters.tcp_rates[0] =
                    ((float)(_openjpeg.image->numcomps * _openjpeg.image->comps[0].w * _openjpeg.image->comps[0].h *
                             _openjpeg.image->comps[0].prec)) /
                    (CINEMA_24_CS * 8 * _openjpeg.image->comps[0].dx * _openjpeg.image->comps[0].dy);
            }
            _openjpeg.parameters.max_comp_size = COMP_24_CS;
            break;
        }
        case CINEMA2K_48:
        {
            for(int i = 0; i < _openjpeg.parameters.tcp_numlayers; ++i)
            {
                _openjpeg.parameters.tcp_rates[0] =
                    ((float)(_openjpeg.image->numcomps * _openjpeg.image->comps[0].w * _openjpeg.image->comps[0].h *
                             _openjpeg.image->comps[0].prec)) /
                    (CINEMA_48_CS * 8 * _openjpeg.image->comps[0].dx * _openjpeg.image->comps[0].dy);
            }
            _openjpeg.parameters.max_comp_size = COMP_48_CS;
            break;
        }
        default:
            break;
    }
    _openjpeg.parameters.cp_disto_alloc = 1;
}

int J2KWriter::initialize4Kpocs(opj_poc_t* POC, int numres)
{
    POC[0].tile = 1;
    POC[0].resno0 = 0;
    POC[0].compno0 = 0;
    POC[0].layno1 = 1;
    POC[0].resno1 = numres - 1;
    POC[0].compno1 = 3;
    POC[0].prg1 = CPRL;
    POC[1].tile = 1;
    POC[1].resno0 = numres - 1;
    POC[1].compno0 = 0;
    POC[1].layno1 = 1;
    POC[1].resno1 = numres;
    POC[1].compno1 = 3;
    POC[1].prg1 = CPRL;
    return 2;
}

void J2KWriter::close()
{
    if(_cio)
    {
        // Output the buffer
        _outFile.write((const char*)_cio->buffer, (std::size_t)cio_tell(_cio));
        // Close and free the byte stream
        opj_cio_close(_cio);
        _cio = NULL;
    }

    _outFile.close();

    if(_openjpeg.image)
    {
        opj_image_destroy(_openjpeg.image);
        _openjpeg.image = NULL;
    }
}
}
}
