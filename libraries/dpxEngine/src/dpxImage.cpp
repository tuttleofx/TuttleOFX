#include "dpxImage.hpp"
#include "include/dpxImage.hpp"
#include "include/dpxUtils.hpp"
#include <string.h>
#include <sstream>
#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
using namespace boost::filesystem;
using namespace tuttle::utils;

namespace tuttle {
namespace io {

DpxImage::DpxImage()
: _data(NULL)
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
    readHeader(f);

    // initialize raw data
    size_t dataSize = 	sizeof(uint32_t) * width() * height();

    // reads and throws away characters until 'offset' characters have been read
    f.ignore(_header._fileInfo.offset - sizeof(_header));
    if ( _data )
        delete [] _data;
    _data = new uint8_t[dataSize];

    // reads data
    if (! f.read(reinterpret_cast<char*>(_data), dataSize)){
        std::ostringstream msg;
        msg << "Unable to read data ";
        msg << "( "<< filename << " )" ;
        throw std::exception();
    }

    f.close();
}

void DpxImage::readHeader(ifstream & f) {

    FileInformation             *gen;
    ImageInformation		*imgif;
    ImageOrientation		*imgor;
    MotionPictureFilm		*mp;
    TelevisionHeader		*tv;

    // reads data from _header...
    if (! f.read(reinterpret_cast<char*>(&_header), sizeof(_header))){
        std::ostringstream msg;
        msg << "Unable to read header...";
        throw std::exception();
    }

    // ...file information
    gen = &(_header._fileInfo);

    gen->magic_num  =	gen->magic_num;
    bool bigEndian = (gen->magic_num == DPX_MAGIC_SWAP);

    if( bigEndian ) {

        //
        // "BIG ENDIAN"
        //
        gen->magic_num				=	swapEndian<unsigned long>(gen->magic_num);
        gen->offset                             =	swapEndian<unsigned long>(gen->offset);
        gen->file_size				=	swapEndian<unsigned long>(gen->file_size);
        gen->ditto_key				=	swapEndian<unsigned long>(1);
        gen->gen_hdr_size			=	swapEndian<unsigned long>(gen->gen_hdr_size);
        gen->ind_hdr_size			=	swapEndian<unsigned long>(gen->ind_hdr_size);
        gen->user_data_size			=	swapEndian<unsigned long>(gen->user_data_size);
        gen->key					=	swapEndian<unsigned long>(gen->key);

        // ...image information
        imgif = &(_header._imageInfo);
        imgif->orientation          =	swapEndian<short>(imgif->orientation);
        imgif->element_number       =	swapEndian<short>(imgif->element_number);
        imgif->pixelsPerLine        =	swapEndian<unsigned long>(imgif->pixelsPerLine);
        imgif->linesPerImageEle     =	swapEndian<unsigned long>(imgif->linesPerImageEle);
        for (int i=0; i<8; i++)
        {
                imgif->image_element[i].data_sign			=	swapEndian<unsigned long>(imgif->image_element[i].data_sign);
                imgif->image_element[i].ref_low_data		=	swapEndian<unsigned long>(imgif->image_element[i].ref_low_data);
                imgif->image_element[i].ref_low_quantity	=	swapEndian<float>(imgif->image_element[i].ref_low_quantity);
                imgif->image_element[i].ref_high_data		=	swapEndian<unsigned long>(imgif->image_element[i].ref_high_data);
                imgif->image_element[i].ref_high_quantity	=	swapEndian<float>(imgif->image_element[i].ref_high_quantity);
                imgif->image_element[i].packing				=	swapEndian<short>(imgif->image_element[i].packing);
                imgif->image_element[i].encoding			=	swapEndian<short>(imgif->image_element[i].encoding);
                imgif->image_element[i].data_offset			=	swapEndian<unsigned long>(imgif->image_element[i].data_offset);
                imgif->image_element[i].eol_padding			=	swapEndian<unsigned long>(imgif->image_element[i].eol_padding);
                imgif->image_element[i].eo_image_padding	=	swapEndian<unsigned long>(imgif->image_element[i].eo_image_padding);
        }

        // ...file orientation
        imgor = &(_header._imageOrientation);
        imgor->x_offset						=	swapEndian<unsigned long>(imgor->x_offset);
        imgor->y_offset						=	swapEndian<unsigned long>(imgor->y_offset);
        imgor->x_center						=	swapEndian<unsigned long>(imgor->x_center);
        imgor->y_center						=	swapEndian<unsigned long>(imgor->y_center);
        imgor->x_orig_size					=	swapEndian<unsigned long>(imgor->x_orig_size);
        imgor->y_orig_size					=	swapEndian<unsigned long>(imgor->y_orig_size);
        imgor->border[0]					=	swapEndian<short>(imgor->border[0]);
        imgor->border[1]					=	swapEndian<short>(imgor->border[1]);
        imgor->border[2]					=	swapEndian<short>(imgor->border[2]);
        imgor->border[3]					=	swapEndian<short>(imgor->border[3]);
        imgor->pixel_aspect[0]				=	swapEndian<unsigned long>(imgor->pixel_aspect[0]);
        imgor->pixel_aspect[1]				=	swapEndian<unsigned long>(imgor->pixel_aspect[1]);

        // ...motion picture film _header
        mp	=	&(_header._motionPicture);
        mp->prefix							=	swapEndian<unsigned long>(mp->prefix);
        mp->count							=	swapEndian<unsigned long>(mp->count);
        mp->frame_position					=	swapEndian<unsigned long>(mp->frame_position);
        mp->sequence_len					=	swapEndian<unsigned long>(mp->sequence_len);
        mp->held_count						=	swapEndian<unsigned long>(mp->held_count);
        mp->frame_rate						=	swapEndian<float>(mp->frame_rate);
        mp->shutter_angle					=	swapEndian<float>(mp->shutter_angle);

        // ...television _header
        tv	=	&(_header._television);
        tv->tim_code						=	swapEndian<unsigned long>(tv->tim_code);
        tv->userBits						=	swapEndian<unsigned long>(tv->userBits);
        tv->hor_sample_rate					=	swapEndian<float>(tv->hor_sample_rate);
        tv->ver_sample_rate					=	swapEndian<float>(tv->ver_sample_rate);
        tv->frame_rate						=	swapEndian<float>(tv->frame_rate);
        tv->time_offset						=	swapEndian<float>(tv->time_offset);
        tv->gamma							=	swapEndian<float>(tv->gamma);
        tv->black_level						=	swapEndian<float>(tv->black_level);
        tv->black_gain						=	swapEndian<float>(tv->black_gain);
        tv->break_point						=	swapEndian<float>(tv->break_point);
        tv->white_level						=	swapEndian<float>(tv->white_level);
        tv->integration_times				=	swapEndian<float>(tv->integration_times);
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

}
}