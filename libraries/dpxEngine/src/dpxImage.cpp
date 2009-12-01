#include "dpxImage.hpp"
#include "include/dpxImage.hpp"
#include "include/dpxUtils.hpp"
#include <string.h>
#include <sstream>
#include <iostream>

#include <cassert>
#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/endian.hpp>
using namespace boost::filesystem;
using namespace tuttle::utils;
using namespace boost;

namespace tuttle {
namespace io {

// @todo: move this in the right place
inline boost::uint16_t reverseBits(boost::uint16_t v) {
    // Get high part
    uint8_t b = ((uint8_t*)&v)[1];
    // Reverse bit from byte (Sean Anderson, July 13, 2001)
    b = (uint8_t)(((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
    uint8_t c = v & 0x00FF;
    c = (uint8_t)(((c * 0x0802LU & 0x22110LU) | (c * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
    v = b | ( uint16_t( c ) << 8 );
    return v;
}

inline boost::uint16_t reverseBytes(boost::uint16_t v) {
    return (((uint8_t*)&v)[1] << 8) | ((uint8_t*)&v)[0];
}

DpxImage::DpxImage()
: _bigEndian(false)
, _data(NULL)
{
    memset( &_header, 0, sizeof(DpxHeader) );
}

DpxImage::~DpxImage() {
    if ( _data )
        delete [] _data;
}

void DpxImage::read(const path & filename) {
    //
    // Open file
    //
    ifstream f(filename, std::ios_base::in | std::ios_base::binary);
    if (!f) {
        std::ostringstream msg;
        msg << "Unable to open ";
        msg << filename;
        throw std::exception();
    }
    readHeader( f );

    // initialize raw data
    size_t sz = dataSize();
    // reads and throws away characters until 'offset' characters have been read
    f.ignore( _header._fileInfo.offset - f.tellg() );
    printf("DPX pixel offset start: %d\n", _header._fileInfo.offset);
    if ( _data )
        delete [] _data;
    _data = new uint8_t[ sz ];

    // reads data
    if ( ! f.read( reinterpret_cast<char*>( _data ), sz ) )
    {
        std::ostringstream msg;
        msg << "Unable to read data ";
        msg << "( "<< filename << " )" ;
        throw std::exception();
    }

    f.close();
}

void DpxImage::readHeader(ifstream & f)
{
    FileInformation             *gen;
    ImageInformation		*imgif;
    ImageOrientation		*imgor;
    MotionPictureFilm		*mp;
    TelevisionHeader		*tv;

    // reads beginning data from _header (we need to get the
    // header size given in file)...
    f.seekg(0, std::ios::beg);
    if (! f.read(reinterpret_cast<char*>(&_header), 32)) {
        std::ostringstream msg;
        msg << "Unable to read header...";
        throw std::exception();
    }

    // ...file information
    gen = &(_header._fileInfo);
    gen->magic_num  =	gen->magic_num;
    _bigEndian = (gen->magic_num == DPX_MAGIC_SWAP);
    if( _bigEndian ) {
        //
        // "BIG ENDIAN"
        //
        gen->magic_num      =	swapEndian<unsigned long>(gen->magic_num);
        gen->offset         =	swapEndian<unsigned long>(gen->offset);
        gen->file_size      =	swapEndian<unsigned long>(gen->file_size);
        gen->ditto_key      =	swapEndian<unsigned long>(1);
        gen->gen_hdr_size   =	swapEndian<unsigned long>(gen->gen_hdr_size);
        gen->ind_hdr_size   =	swapEndian<unsigned long>(gen->ind_hdr_size);
    }
    size_t hdrSize = ( gen->gen_hdr_size > 0 ? gen->gen_hdr_size : sizeof(DpxHeader) ) - 32;

    // reads data from _header...
    if (! f.read(reinterpret_cast<char*>(&_header)+32, hdrSize)){
        std::ostringstream msg;
        msg << "Unable to read header...";
        throw std::exception();
    }

    if( _bigEndian ) {
        //
        // "BIG ENDIAN"
        //
        gen->user_data_size =	swapEndian<unsigned long>(gen->user_data_size);
        gen->key            =	swapEndian<unsigned long>(gen->key);

        // ...image information
        imgif = &(_header._imageInfo);
        imgif->orientation      = swapEndian<short>(imgif->orientation);
        imgif->element_number   = swapEndian<short>(imgif->element_number);
        imgif->pixelsPerLine    = swapEndian<unsigned long>(imgif->pixelsPerLine);
        imgif->linesPerImageEle = swapEndian<unsigned long>(imgif->linesPerImageEle);
        for (int i=0; i<8; i++)
        {
                imgif->image_element[i].data_sign           = swapEndian<unsigned long>(imgif->image_element[i].data_sign);
                imgif->image_element[i].ref_low_data        = swapEndian<unsigned long>(imgif->image_element[i].ref_low_data);
                imgif->image_element[i].ref_low_quantity    = swapEndian<float>(imgif->image_element[i].ref_low_quantity);
                imgif->image_element[i].ref_high_data       = swapEndian<unsigned long>(imgif->image_element[i].ref_high_data);
                imgif->image_element[i].ref_high_quantity   = swapEndian<float>(imgif->image_element[i].ref_high_quantity);
                imgif->image_element[i].packing             = swapEndian<short>(imgif->image_element[i].packing);
                imgif->image_element[i].encoding            = swapEndian<short>(imgif->image_element[i].encoding);
                imgif->image_element[i].data_offset         = swapEndian<unsigned long>(imgif->image_element[i].data_offset);
                imgif->image_element[i].eol_padding         = swapEndian<unsigned long>(imgif->image_element[i].eol_padding);
                imgif->image_element[i].eo_image_padding    = swapEndian<unsigned long>(imgif->image_element[i].eo_image_padding);
        }

        // ...file orientation
        imgor = &(_header._imageOrientation);
        imgor->x_offset =   swapEndian<unsigned long>(imgor->x_offset);
        imgor->y_offset =   swapEndian<unsigned long>(imgor->y_offset);
        imgor->x_center =   swapEndian<unsigned long>(imgor->x_center);
        imgor->y_center =   swapEndian<unsigned long>(imgor->y_center);
        imgor->x_orig_size  =   swapEndian<unsigned long>(imgor->x_orig_size);
        imgor->y_orig_size  =   swapEndian<unsigned long>(imgor->y_orig_size);
        imgor->border[0]    =   swapEndian<short>(imgor->border[0]);
        imgor->border[1]    =   swapEndian<short>(imgor->border[1]);
        imgor->border[2]    =   swapEndian<short>(imgor->border[2]);
        imgor->border[3]    =   swapEndian<short>(imgor->border[3]);
        imgor->pixel_aspect[0]  =   swapEndian<unsigned long>(imgor->pixel_aspect[0]);
        imgor->pixel_aspect[1]  =   swapEndian<unsigned long>(imgor->pixel_aspect[1]);

        // ...motion picture film _header
        mp                  =	&(_header._motionPicture);
        mp->prefix          =	swapEndian<unsigned long>(mp->prefix);
        mp->count           =	swapEndian<unsigned long>(mp->count);
        mp->frame_position  =	swapEndian<unsigned long>(mp->frame_position);
        mp->sequence_len    =	swapEndian<unsigned long>(mp->sequence_len);
        mp->held_count      =	swapEndian<unsigned long>(mp->held_count);
        mp->frame_rate      =	swapEndian<float>(mp->frame_rate);
        mp->shutter_angle   =	swapEndian<float>(mp->shutter_angle);

        // ...television _header
        tv                      =   &(_header._television);
        tv->tim_code            =   swapEndian<unsigned long>(tv->tim_code);
        tv->userBits            =   swapEndian<unsigned long>(tv->userBits);
        tv->hor_sample_rate     =   swapEndian<float>(tv->hor_sample_rate);
        tv->ver_sample_rate     =	swapEndian<float>(tv->ver_sample_rate);
        tv->frame_rate          =	swapEndian<float>(tv->frame_rate);
        tv->time_offset         =	swapEndian<float>(tv->time_offset);
        tv->gamma               =	swapEndian<float>(tv->gamma);
        tv->black_level         =	swapEndian<float>(tv->black_level);
        tv->black_gain          =	swapEndian<float>(tv->black_gain);
        tv->break_point         =	swapEndian<float>(tv->break_point);
        tv->white_level         =	swapEndian<float>(tv->white_level);
        tv->integration_times   =	swapEndian<float>(tv->integration_times);
    }

    if(_header._imageInfo.orientation > 1){
        std::ostringstream msg;
        msg << "bad orientation value";
        throw std::exception();
    }

    uint8_t bitSize = _header._imageInfo.image_element[0].bit_size;
    if(bitSize != 8 && bitSize != 10)
    {
        std::ostringstream msg;
        msg << "bad bit size value";
        throw std::exception();
    }

    //TODO dpx packing
    uint16_t packing = _header._imageInfo.image_element[0].packing;
    if (packing == 256) packing = 1;
    if (packing != 0 && packing != 1 && packing != 5)
    {
        std::ostringstream msg;
        msg << "bad packing value";
        throw std::exception();
    }
}

void DpxImage::readHeader(const path & filename) {
    //
    // Open file
    //
    ifstream f(filename, std::ios_base::in | std::ios_base::binary);
    if (!f) {
        std::ostringstream msg;
        msg << "Unable to open ";
        msg << filename;
        throw std::exception();
    }

    readHeader(f);
    f.close();
}

void DpxImage::write(const path & filename) {

}

/// HAVE TO BE DELETED BY CALLER
const uint16_t *DpxImage::data16() const {
    size_t nComponents = components();
    uint8_t bits = _header._imageInfo.image_element[0].bit_size;
    size_t w = width();
    size_t h = height();
    uint16_t *pData = new uint16_t[w * h * nComponents];
    printf("Number of components: %d\n", nComponents);
    switch (bits) {
    case 10:
    {
        size_t x;
        // uint32_t channel size
        size_t n = w * h;
        uint32_t *pData32 = (uint32_t *)_data;
        switch ( packing() )
        {
            case 0:
            {
                for (size_t y = 0; y < h; ++y)
                {
                    size_t start = y * (w * nComponents);
                    size_t stop = start + (w * nComponents);
                    for (x = start; x < stop; ++x) {
                        unsigned a = (x * bits) / (sizeof(uint32_t) * 8);
                        // b bits greater than 32*k and lesser than 32*(k+1)
                        unsigned b = (x * bits) % (sizeof(uint32_t) * 8);
                        if ( b > 22 ) {
                            /*
                            // Bits are between current pData32 and next one
                            pData[x] = ((_bigEndian ? swapEndian<uint32_t>( pData32[a + 1] ) : pData32[a + 1] >> b) << 6) |
                                       ((( _bigEndian ? swapEndian<uint32_t>( pData32[a] ) : pData32[a] >> ( 32 - b ) ) & 0x3ff) << 6);
                            */
                        } else {
                            pData[x] = (( _bigEndian ? swapEndian<uint32_t>( pData32[a] ) : pData32[a] >> (32-(b+bits)) ) & 0x3ff) << 6;
                        }
                    }
                }
                break;
            }
            case 1:
            {
                uint32_t val;
                // Check if pixels are big endian
                if ( _bigEndian ) {
                    // check if host is big endian
                    if ( BOOST_BYTE_ORDER == 4321 ) {
                        for (x = 0; x < n; x++) {
                            val = pData32[x];
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 22) & 0x03ff);
                            pData[3 * x + 1] = (uint16_t)((val >> 12) & 0x03ff);
                            pData[3 * x + 2] = (uint16_t)((val >> 02) & 0x03ff);
                        }
                    } else {
                        for (x = 0; x < n; x++) {
                            val = swapEndian<uint32_t>( pData32[x] );
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 16) & 0xffc0);
                            pData[3 * x + 1] = (uint16_t)((val >> 06) & 0xffc0);
                            pData[3 * x + 2] = (uint16_t)((val << 04) & 0xffc0);
                        }
                    }
                } else {
                    // check if host is big endian
                    if ( BOOST_BYTE_ORDER == 4321 ) {
                        for (x = 0; x < n; x++) {
                            val = swapEndian<uint32_t>( pData32[x] );
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 22) & 0x03ff);
                            pData[3 * x + 1] = (uint16_t)((val >> 12) & 0x03ff);
                            pData[3 * x + 2] = (uint16_t)((val >> 02) & 0x03ff);
                        }
                    } else {
                        for (x = 0; x < n; x++) {
                            val = pData32[x];
                            pData[3 * x + 0] = (uint16_t)((val >> 16) & 0xffc0);
                            pData[3 * x + 1] = (uint16_t)((val >> 06) & 0xffc0);
                            pData[3 * x + 2] = (uint16_t)((val << 04) & 0xffc0);
                        }
                    }
                }
                break;
            }
            case 2: {
                // @todo check the following code
                uint32_t val;
                // Check if pixels are big endian
                if ( _bigEndian ) {
                    // check if host is big endian
                    if ( BOOST_BYTE_ORDER == 4321 ) {
                        for (x = 0; x < n; x++) {
                            val = pData32[x];
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 20) & 0x03ff);
                            pData[3 * x + 1] = (uint16_t)((val >> 10) & 0x03ff);
                            pData[3 * x + 2] = (uint16_t)((val >> 00) & 0x03ff);
                        }
                    } else {
                        for (x = 0; x < n; x++) {
                            val = swapEndian<uint32_t>( pData32[x] );
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 14) & 0xffc0);
                            pData[3 * x + 1] = (uint16_t)((val >> 04) & 0xffc0);
                            pData[3 * x + 2] = (uint16_t)((val << 06) & 0xffc0);
                        }
                    }
                } else {
                    // check if host is big endian
                    if ( BOOST_BYTE_ORDER == 4321 ) {
                        for (x = 0; x < n; x++) {
                            val = swapEndian<uint32_t>( pData32[x] );
                            // we convert values from 10b to interpretable 16b
                            pData[3 * x + 0] = (uint16_t)((val >> 20) & 0x03ff);
                            pData[3 * x + 1] = (uint16_t)((val >> 10) & 0x03ff);
                            pData[3 * x + 2] = (uint16_t)((val >> 00) & 0x03ff);
                        }
                    } else {
                        for (x = 0; x < n; x++) {
                            val = pData32[x];
                            pData[3 * x + 0] = (uint16_t)((val >> 14) & 0xffc0);
                            pData[3 * x + 1] = (uint16_t)((val >> 04) & 0xffc0);
                            pData[3 * x + 2] = (uint16_t)((val << 06) & 0xffc0);
                        }
                    }
                }
                break;
            }
        }
    }
      /*
      case 12:
        switch (e.packing) {
          case 0: {
            unsigned n = (e.bytes + 3) / 4;
            ARRAY(U32, src, n);
            read(src, e.dataOffset + y * e.bytes, e.bytes);
            if (flipped)
              flip(src, n);
            for (unsigned x = 0; x < width * e.components; x++) {
              unsigned a = (x * 12) / 32;
              unsigned b = (x * 12) % 32;
              if (b > 20)
                buf[x] = ((src[a + 1] << (32 - b)) + (src[a] >> b)) & 0xfff;
              else
                buf[x] = (src[a] >> b) & 0xfff;
            }
            break;
          }
          case 1: {
            unsigned n = width * e.components;
            read(buf, e.dataOffset + y * e.bytes, n * 2);
            if (flipped)
              flip(buf, n);
            for (unsigned x = 0; x < n; x++)
              buf[x] >>= 4;
            break;
          }
          case 2: {
            unsigned n = width * e.components;
            read(buf, e.dataOffset + y * e.bytes, n * 2);
            if (flipped)
              flip(buf, n);
            for (unsigned x = 0; x < n; x++)
              buf[x] &= 0xfff;
            break;
          }
        }
        break;
      case 16:
        read(buf, e.dataOffset + y * e.bytes, width * e.components * 2);
        if (flipped)
          flip(buf, width * e.components);
        break;
       */
    }

    return pData;
}

}
}
