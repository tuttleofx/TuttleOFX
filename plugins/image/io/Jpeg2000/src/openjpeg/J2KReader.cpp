#include "J2KReader.hpp"
#include "J2KCommon.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include "ofxsMemory.h"
#include "ofxsCore.h"

#include <openjpeg.h>

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <cstring>

namespace tuttle
{
namespace io
{

namespace fs = boost::filesystem;

J2KReader::J2KReader()
{
    _fileData = NULL;
    _dataLength = 0;
    memset(&_openjpeg, 0, sizeof(OpenJpegStuffs));
}

J2KReader::~J2KReader()
{
    close();
}

void J2KReader::open(const std::string& filename)
{
    close();
    if(!fs::exists(filename))
    {
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::filename(filename));
    }

    fs::ifstream inputDataStream;
    inputDataStream.open(filename, std::ios_base::binary);

    if(!inputDataStream.is_open())
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Unable to open file.") << exception::filename(filename));
    }
    uint32_t magic;

    // read the input file and put it in memory
    // ----------------------------------------
    inputDataStream.read((char*)&magic, sizeof(int));
    if(inputDataStream.fail())
    {
        inputDataStream.close();
        BOOST_THROW_EXCEPTION(exception::Value() << exception::dev("Unable to read magic number.")
                                                 << exception::filename(filename));
    }
    if(magic != MAYBE_MAGIC && magic != MAYBE_REV_MAGIC)
    {
        inputDataStream.close();
        BOOST_THROW_EXCEPTION(exception::Value() << exception::dev("Invalid magic number.")
                                                 << exception::filename(filename));
    }

    inputDataStream.seekg(0, std::ios::end);
    std::ssize_t dataLength = inputDataStream.tellg();
    inputDataStream.seekg(0, std::ios::beg);
    if(dataLength != _dataLength || !_fileData)
    {
        if(_fileData)
        {
            OFX::memory::free(_fileData);
        }
        _fileData = (uint8_t*)OFX::memory::allocate(dataLength);
    }
    inputDataStream.read((char*)_fileData, dataLength);
    if(inputDataStream.fail())
    {
        inputDataStream.close();
        OFX::memory::free(_fileData);
        _fileData = NULL;
        _dataLength = 0;
        BOOST_THROW_EXCEPTION(exception::Value() << exception::dev("Unable to read image data.")
                                                 << exception::filename(filename));
    }
    inputDataStream.close();
    _dataLength = dataLength;
}

void J2KReader::decode(bool headeronly)
{
    if(!_fileData || !_dataLength)
    {
        BOOST_THROW_EXCEPTION(exception::Bug() << exception::dev("Need to open the file before decoding."));
    }
    opj_dparameters_t parameters; // decompression parameters
    opj_dinfo_t* dinfo = NULL;    // handle to a decompressor
    opj_cio_t* cio = NULL;

    _openjpeg.event_mgr.error_handler = NULL;
    _openjpeg.event_mgr.warning_handler = NULL;
    _openjpeg.event_mgr.info_handler = NULL;
    opj_set_default_decoder_parameters(&parameters);

    if(headeronly)
    {
        parameters.cp_limit_decoding = LIMIT_TO_MAIN_HEADER;
    }

    // Decompress a JPEG-2000 codestream
    // get a decoder handle
    dinfo = opj_create_decompress(CODEC_J2K);
    // Catch events using our callbacks and give a local context
    opj_set_event_mgr((opj_common_ptr)dinfo, &_openjpeg.event_mgr, stderr);
    // setup the decoder decoding parameters using user parameters
    opj_setup_decoder(dinfo, &parameters);
    if(!dinfo)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::dev("Failed to open decoder for image."));
    }
    // open a byte stream
    cio = opj_cio_open((opj_common_ptr)dinfo, _fileData, _dataLength);
    if(!cio)
    {
        opj_destroy_decompress(dinfo);
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::dev("Failed to open decoder for image."));
    }
    // Start decoding to get an image
    if(_openjpeg.image)
    {
        opj_image_destroy(_openjpeg.image);
    }
    _openjpeg.image = opj_decode(dinfo, cio);
    // close the byte stream
    opj_destroy_decompress(dinfo);
    opj_cio_close(cio);
    if(!_openjpeg.image)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::dev("Failed to decode image."));
    }
}

void J2KReader::close()
{
    if(_openjpeg.image)
    {
        opj_image_destroy(_openjpeg.image);
        _openjpeg.image = NULL;
    }

    if(_fileData)
    {
        OFX::memory::free(_fileData);
        _fileData = NULL;
    }
    _dataLength = 0;
    memset(&_openjpeg, 0, sizeof(OpenJpegStuffs));
}
}
}
