/* 
 * File:   J2KEngine.cpp
 * Author: edubois
 * 
 * Created on March 26, 2010, 10:19 AM
 */

#include "J2KReader.hpp"
#include "J2KCommon.hpp"
#include "tuttle/common/utils/global.hpp"
#include "ofxsMemory.h"
#include "ofxsCore.h"
#include <openjpeg.h>

#include <boost/filesystem/fstream.hpp>
#include <cstdlib>
#include <cstring>

namespace tuttle {
namespace io {

using namespace std;
using namespace boost::filesystem;

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

void J2KReader::open(const std::string & filename)
{
	close();
	if( exists( filename ) )
	{
		boost::filesystem::ifstream inputDataStream;
		inputDataStream.open(filename);
		if( inputDataStream.is_open() )
		{
            uint32_t magic;

            // read the input file and put it in memory
            // ----------------------------------------
			inputDataStream.read((char*)&magic, sizeof( int ));
            if( inputDataStream.fail() )
            {
                inputDataStream.close();
				throw( std::logic_error( std::string("Unable to read magic number on ") + filename ) );
            }
            if( magic != MAYBE_MAGIC && magic != MAYBE_REV_MAGIC )
            {
                inputDataStream.close();
				throw( std::logic_error( std::string("Invalid magic number on ") + filename ) );
            }

            inputDataStream.seekg(0, ios::end);
            size_t dataLength = inputDataStream.tellg();
            inputDataStream.seekg(0, ios::beg);
			if (dataLength != _dataLength || !_fileData)
			{
				if (_fileData)
					OFX::Memory::free(_fileData);
				_fileData = (uint8_t*)OFX::Memory::allocate(dataLength);
			}
            inputDataStream.read( (char*)_fileData, dataLength );
            if( inputDataStream.fail() )
            {
                inputDataStream.close();
				OFX::Memory::free(_fileData);
				_fileData = NULL;
				_dataLength = 0;
				throw( std::logic_error(std::string("Unable to read image data on ") + filename ));
            }
			inputDataStream.close();
			_dataLength = dataLength;
         }
         else
         {
			throw(std::logic_error(std::string("Unable to open file ") + filename));
         }
	}
	else
	{
		throw(std::logic_error(std::string("File ") + filename + " doesn't exists!"));
	}
}

void J2KReader::decode()
{
	if (!_fileData || !_dataLength)
	{
		throw(std::logic_error("open a file before decoding!"));
	}
	else
	{
		opj_dparameters_t       parameters;       // decompression parameters
		opj_dinfo_t             *dinfo = NULL;    // handle to a decompressor
		opj_cio_t               *cio = NULL;

		_openjpeg.event_mgr.error_handler = NULL;
		_openjpeg.event_mgr.warning_handler = NULL;
		_openjpeg.event_mgr.info_handler = NULL;
		opj_set_default_decoder_parameters(&parameters);

		// Decompress a JPEG-2000 codestream
		// get a decoder handle
		dinfo = opj_create_decompress(CODEC_J2K);
		// Catch events using our callbacks and give a local context
		opj_set_event_mgr((opj_common_ptr) dinfo, &_openjpeg.event_mgr, stderr );
		// setup the decoder decoding parameters using user parameters
		opj_setup_decoder(dinfo, &parameters);
		if ( !dinfo )
		{
			OFX::Memory::free(_fileData);
			_fileData = NULL;
			_dataLength = 0;
			throw(std::logic_error("Failed to open decoder for image!"));
		}
		// open a byte stream
		cio = opj_cio_open((opj_common_ptr)dinfo, _fileData, _dataLength);
		if (!cio)
		{
			OFX::Memory::free(_fileData);
			_fileData = NULL;
			_dataLength = 0;
			opj_destroy_decompress( dinfo );
			throw(std::logic_error("Failed to open decoder for image!"));
		}
		// Start decoding to get an image
		_openjpeg.image = opj_decode( dinfo, cio );
		if ( !_openjpeg.image )
		{
			opj_destroy_decompress( dinfo );
			opj_cio_close( cio );
			OFX::Memory::free(_fileData);
			_fileData = NULL;
			_dataLength = 0;
			throw(std::logic_error("Failed to decode image!"));
		}
		// close the byte stream
		opj_cio_close( cio );
		opj_destroy_decompress( dinfo );
		OFX::Memory::free(_fileData);
		_fileData = NULL;
		_dataLength = 0;
	}
}

void J2KReader::close()
{
	if ( _openjpeg.image )
	{
		opj_image_destroy( _openjpeg.image );
		_openjpeg.image = NULL;
	}

	if (_fileData)
	{
		OFX::Memory::free(_fileData);
		_fileData = NULL;
	}
	_dataLength = 0;
	memset(&_openjpeg, 0, sizeof(OpenJpegStuffs));
}

}
}
