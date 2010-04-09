#include "tuttle/common/utils/global.hpp"
#include "dpxImage.hpp"
#include "dpxUtils.hpp"

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/endian.hpp>
#include <boost/scoped_array.hpp>

#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cassert>

using namespace boost::filesystem;
using namespace tuttle::utils;
using namespace boost;

namespace tuttle {
namespace io {

//////////////////////////////////// CUT THIS PART AND RELATIVES ON RELEASE
template < typename T >
inline T highbit( T& t )
{
	return t = ( ( (T)( -1 ) ) >> 1 ) + 1;
}

template < typename T >
std::ostream& bin( T& value, std::ostream& o )
{
	for( T bit = highbit( bit ); bit; bit >>= 1 )
	{
		o << ( ( value & bit ) ? '1' : '0' );
	}
	return o;
}

// @todo: move this in the right place
inline boost::uint16_t reverseBits( boost::uint16_t v )
{
	// Get high part
	uint8_t b = ( (uint8_t*)&v )[1];

	// Reverse bit from byte (Sean Anderson, July 13, 2001)
	b = (uint8_t)( ( ( b * 0x0802LU & 0x22110LU ) | ( b * 0x8020LU & 0x88440LU ) ) * 0x10101LU >> 16 );
	uint8_t c = v & 0x00FF;
	c = (uint8_t)( ( ( c * 0x0802LU & 0x22110LU ) | ( c * 0x8020LU & 0x88440LU ) ) * 0x10101LU >> 16 );
	v = b | ( uint16_t( c ) << 8 );
	return v;
}

inline boost::uint16_t reverseBytes( boost::uint16_t v )
{
	return ( ( (uint8_t*)&v )[1] << 8 ) | ( (uint8_t*)&v )[0];
}

//////////////////////////////////// END CUT

DpxImage::DpxImage()
{}

DpxImage::~DpxImage()
{}

void DpxImage::read( const path& filename, bool reinterpretation )
{
	//
	// Open file
	//
	ifstream f( filename, std::ios_base::in | std::ios_base::binary );

	if( !f )
	{
		std::ostringstream msg;
		msg << "Unable to open ";
		msg << filename;
		throw std::exception();
	}
	readHeader( f );

	// initialize raw data
	_dataSize = dataSize();
	if (_dataSize != 0)
	{
		// read and throws away characters until 'offset' characters have been read
		f.ignore( _header.dataOffset() - f.tellg() );
		// Data have to be packed on uint32_t size to allow indianess fast
		// reinterpretation
		_data.reset( new uint8_t[ _dataSize + ( _dataSize % sizeof( uint32_t ) ) ] );
		// read data
		if( !f.read( reinterpret_cast<char*>( _data.get() ), _dataSize ) )
		{
			std::ostringstream msg;
			msg << "Unable to read data ";
			msg << "( " << filename << " )" ;
			throw std::exception();
		}
		if( reinterpretation && isEndianReinterpNeeded() )
		{
			_indyData.reset( reinterpretEndianness() );
		}
		else
		{
			_indyData = _data;
		}
	}
	else
	{
		std::ostringstream msg;
		msg << "Unhandled dpx data type.";
		msg << "( " << filename << " )" ;
		throw std::exception();
	}
	f.close();
}

void DpxImage::readHeader( const path& filename )
{
	//
	// Open file
	//
	ifstream f( filename, std::ios_base::in | std::ios_base::binary );

	if( !f )
	{
		std::ostringstream msg;
		msg << "Unable to open ";
		msg << filename;
		std::cerr << msg << std::endl;
		throw std::exception();
	}

	readHeader( f );
	f.close();
}

void DpxImage::readHeader( ifstream& f )
{
	FileInformation* gen;

	// reads beginning data from _header (we need to get the
	// header size given in file)...
	f.seekg( 0, std::ios::beg );
	if( !f.read( reinterpret_cast<char*>( &_header ), 36 ) )
	{
		std::ostringstream msg;
		msg << "Unable to read header...";
		std::cerr << "Unable to read dpx header..." << std::endl;
		throw std::exception();
	}

	// ...file information
	gen            = &( _header._fileInfo );
	_header._fileInfo.magic_num = gen->magic_num;
	size_t hdrSize = 0;

	if( _header._fileInfo.magic_num == DPX_MAGIC_SWAP )
	{
		uint32_t genhdrsize = swapEndian<uint32_t>( gen->gen_hdr_size );
		hdrSize = ( genhdrsize > 0 ? genhdrsize : sizeof( DpxHeader ) ) - 36;
	}
	else if (_header._fileInfo.magic_num == DPX_MAGIC)
	{
		hdrSize = ( gen->gen_hdr_size > 0 ? gen->gen_hdr_size : sizeof( DpxHeader ) ) - 36;
	}
	else
	{
		throw(std::logic_error("Not a dpx image !"));
	}
	// Read meta dynamic infos
	scoped_array<uint8_t> _hdrBuffer(new uint8_t[hdrSize]);
	// reads data from _header...
	if( !f.read( (char*)_hdrBuffer.get(), hdrSize ) )
	{
		std::ostringstream msg;
		msg << "DPX: Unable to read header...";
		std::cerr << msg << std::endl;
		throw std::exception();
	}
	// Read dynamic data
	size_t bufpos = 0;
	readDynamicHdrData((uint8_t*)gen->file_name, sizeof(gen->file_name), _hdrBuffer.get(), bufpos);
	bufpos += sizeof(gen->file_name);
	readDynamicHdrData((uint8_t*)gen->create_time, sizeof(gen->create_time), _hdrBuffer.get(), bufpos);
	bufpos += sizeof(gen->create_time);
	readDynamicHdrData((uint8_t*)gen->creator, sizeof(gen->creator), _hdrBuffer.get(), bufpos);
	bufpos += sizeof(gen->creator);
	readDynamicHdrData((uint8_t*)gen->project, sizeof(gen->project), _hdrBuffer.get(), bufpos);
	bufpos += sizeof(gen->project);
	readDynamicHdrData((uint8_t*)gen->copyright, sizeof(gen->copyright), _hdrBuffer.get(), bufpos);
	bufpos += sizeof(gen->copyright);
	memcpy(&gen->key, _hdrBuffer.get() + bufpos, sizeof(uint32_t));
	bufpos += sizeof(uint32_t);
	memcpy(&gen->reserved, _hdrBuffer.get() + bufpos, sizeof(gen->reserved));
	bufpos += sizeof(gen->reserved);
	// Copy the remaining header infos
	memcpy(&(_header._imageInfo), _hdrBuffer.get() + bufpos, hdrSize - bufpos);

	if ( _header._fileInfo.magic_num == DPX_MAGIC_SWAP )
	{
		_header.swapHeader();
	}
	if( _header._imageInfo.orientation > 1 )
	{
		std::ostringstream msg;
		msg << "DPX: bad orientation value";
		std::cerr << msg << std::endl;
		throw std::exception();
	}

	uint8_t bitSize = _header._imageInfo.image_element[0].bit_size;
	if( bitSize != 8 && bitSize != 10 && bitSize != 12 && bitSize != 16 )
	{
		std::ostringstream msg;
		msg << "DPX: bad bit size value (= " << bitSize << ")";
		std::cerr << msg << std::endl;
		throw std::exception();
	}

	uint16_t packing = _header._imageInfo.image_element[0].packing;
	if( packing == 256 )
		packing = 1;
	if( packing != 0 && packing != 1 && packing != 5 )
	{
		std::ostringstream msg;
		msg << "bad packing value";
		std::cerr << msg << std::endl;
		throw std::exception();
	}
}

void DpxHeader::swapHeader()
{
	//
	// "BIG ENDIAN"
	//
	_fileInfo.offset         =   swapEndian<uint32_t>( _fileInfo.offset );
	_fileInfo.file_size      =   swapEndian<uint32_t>( _fileInfo.file_size );
	_fileInfo.ditto_key      =   swapEndian<uint32_t>( 1 );
	_fileInfo.gen_hdr_size   =   swapEndian<uint32_t>( _fileInfo.gen_hdr_size );
	_fileInfo.ind_hdr_size   =   swapEndian<uint32_t>( _fileInfo.ind_hdr_size );
	_fileInfo.user_data_size =   swapEndian<boost::uint32_t>( _fileInfo.user_data_size );
	_fileInfo.key            =   swapEndian<boost::uint32_t>( _fileInfo.key );

	// ...image information
	_imageInfo.orientation      = swapEndian<boost::uint16_t>( _imageInfo.orientation );
	_imageInfo.element_number   = swapEndian<boost::uint16_t>( _imageInfo.element_number );
	_imageInfo.pixelsPerLine    = swapEndian<boost::uint32_t>( _imageInfo.pixelsPerLine );
	_imageInfo.linesPerImageEle = swapEndian<boost::uint32_t>( _imageInfo.linesPerImageEle );

	for( size_t i = 0; i < 8; ++i )
	{
		_imageInfo.image_element[i].data_sign         = swapEndian<uint32_t>( _imageInfo.image_element[i].data_sign );
		_imageInfo.image_element[i].ref_low_data      = swapEndian<uint32_t>( _imageInfo.image_element[i].ref_low_data );
		_imageInfo.image_element[i].ref_low_quantity  = swapEndian<float>( _imageInfo.image_element[i].ref_low_quantity );
		_imageInfo.image_element[i].ref_high_data     = swapEndian<uint32_t>( _imageInfo.image_element[i].ref_high_data );
		_imageInfo.image_element[i].ref_high_quantity = swapEndian<float>(_imageInfo.image_element[i].ref_high_quantity );
		_imageInfo.image_element[i].packing           = swapEndian<short>(_imageInfo.image_element[i].packing );
		_imageInfo.image_element[i].encoding          = swapEndian<short>(_imageInfo.image_element[i].encoding );
		_imageInfo.image_element[i].data_offset       = swapEndian<uint32_t>(_imageInfo.image_element[i].data_offset );
		_imageInfo.image_element[i].eol_padding       = swapEndian<uint32_t>(_imageInfo.image_element[i].eol_padding );
		_imageInfo.image_element[i].eo_image_padding  = swapEndian<uint32_t>(_imageInfo.image_element[i].eo_image_padding );
	}

	//..file orientation
	_imageOrientation.x_offset        =   swapEndian<uint32_t>(_imageOrientation.x_offset );
	_imageOrientation.y_offset        =   swapEndian<uint32_t>(_imageOrientation.y_offset );
	_imageOrientation.x_center        =   swapEndian<uint32_t>(_imageOrientation.x_center );
	_imageOrientation.y_center        =   swapEndian<uint32_t>(_imageOrientation.y_center );
	_imageOrientation.x_orig_size     =   swapEndian<uint32_t>(_imageOrientation.x_orig_size );
	_imageOrientation.y_orig_size     =   swapEndian<uint32_t>(_imageOrientation.y_orig_size );
	_imageOrientation.border[0]       =   swapEndian<short>(_imageOrientation.border[0] );
	_imageOrientation.border[1]       =   swapEndian<short>(_imageOrientation.border[1] );
	_imageOrientation.border[2]       =   swapEndian<short>(_imageOrientation.border[2] );
	_imageOrientation.border[3]       =   swapEndian<short>(_imageOrientation.border[3] );
	_imageOrientation.pixel_aspect[0] =   swapEndian<uint32_t>(_imageOrientation.pixel_aspect[0] );
	_imageOrientation.pixel_aspect[1] =   swapEndian<uint32_t>(_imageOrientation.pixel_aspect[1] );

	//..motion picture film 
	_motionPicture.prefix         =   swapEndian<uint32_t>(_motionPicture.prefix );
	_motionPicture.count          =   swapEndian<uint32_t>(_motionPicture.count );
	_motionPicture.frame_position =   swapEndian<uint32_t>(_motionPicture.frame_position );
	_motionPicture.sequence_len   =   swapEndian<uint32_t>(_motionPicture.sequence_len );
	_motionPicture.held_count     =   swapEndian<uint32_t>(_motionPicture.held_count );
	_motionPicture.frame_rate     =   swapEndian<float>(_motionPicture.frame_rate );
	_motionPicture.shutter_angle  =   swapEndian<float>(_motionPicture.shutter_angle );

	//..television 
	_television.tim_code          =   swapEndian<uint32_t>(_television.tim_code );
	_television.userBits          =   swapEndian<uint32_t>(_television.userBits );
	_television.hor_sample_rate   =   swapEndian<float>(_television.hor_sample_rate );
	_television.ver_sample_rate   =   swapEndian<float>(_television.ver_sample_rate );
	_television.frame_rate        =   swapEndian<float>(_television.frame_rate );
	_television.time_offset       =   swapEndian<float>(_television.time_offset );
	_television.gamma             =   swapEndian<float>(_television.gamma );
	_television.black_level       =   swapEndian<float>(_television.black_level );
	_television.black_gain        =   swapEndian<float>(_television.black_gain );
	_television.break_point       =   swapEndian<float>(_television.break_point );
	_television.white_level       =   swapEndian<float>(_television.white_level );
	_television.integration_times =   swapEndian<float>(_television.integration_times );
}

size_t DpxImage::readDynamicHdrData(uint8_t *dst, size_t maxLen, uint8_t *buffer, size_t bufpos)
{
	// Strings always
	buffer += bufpos;
	strncpy((char*)dst, (char*)buffer, maxLen);
	int startPos = bufpos;
	bufpos += strnlen((char*)dst, 100);
	// First skip 0x00 and following 0xFF caracters
	while( buffer[bufpos] == 0x00 && (bufpos - startPos) < maxLen )
	{
		bufpos++;
	}
	while( buffer[bufpos] == 0xFF && (bufpos - startPos) < maxLen )
	{
		bufpos++;
	}
	return bufpos;
}

void DpxImage::write( const path& filename )
{
	//
	// Open file
	//
	ofstream f( filename, std::ios_base::out | std::ios_base::binary );

	if( !f )
	{
		std::ostringstream msg;
		msg << "Unable to open ";
		msg << filename;
		std::cerr << msg << std::endl;
		throw std::exception();
	}
	_header.setDataOffset( 2048 );
	writeHeader( f );
	// Pad with zeros untill we get at the right offset
	std::vector<uint8_t> zeros( _header.dataOffset() - f.tellp() );
	if( zeros.size() )
	{
		if( !f.write( reinterpret_cast<char*>( &zeros[0] ), zeros.size() ) )
		{
			throw std::exception();
		}
	}

	// write image data
	if( !f.write( reinterpret_cast<char*>( _data.get() ), _dataSize ) )
	{
		std::ostringstream msg;
		msg << "Unable to read data ";
		msg << "( " << filename << " )" ;
		throw std::exception();
	}
	f.close();
}

void DpxImage::writeHeader( ofstream& f )
{
	// Write data
	DpxHeader header = _header;
	if ( header.bigEndian() )
	{
		header.swapHeader();
	}
	if( !f.write( reinterpret_cast<char*>( &header._fileInfo ), sizeof( FileInformation ) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (FileInformation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &header._imageInfo ), sizeof( ImageInformation ) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (ImageInformation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &header._imageOrientation ), sizeof( ImageOrientation ) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (ImageOrientation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &header._motionPicture ), sizeof( MotionPictureFilm ) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (MotionPictureFilm)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &header._television ), sizeof( TelevisionHeader ) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (TelevisionHeader)" ;
		throw std::exception();
	}
}

bool DpxImage::isEndianReinterpNeeded() const
{
	if( _header.bigEndian() && BOOST_BYTE_ORDER == 1234 )
	{
		switch( _header.bitSize() )
		{
			// 8 bits doesn't need convertion
			case 8:
			case 16:
				return false;
				break;
			default:
				return true;
		}
	}
	return false;
}

uint8_t* DpxImage::reinterpretEndianness() const
{
	// Do we need reinterpretation ?
	uint8_t* pData = NULL;

	if( _header.bigEndian() && BOOST_BYTE_ORDER == 1234 )
	{
		switch( _header.bitSize() )
		{
			// 8 bits doesn't need convertion
			case 8:
				break;
			// Need short swap
			case 12:
			{
				// Data have to be packed on uint32_t size to allow indianess fast
				// reinterpretation
				pData = new uint8_t[_dataSize + ( _dataSize % sizeof( uint32_t ) )];
				size_t dataSize16    = ( _dataSize + ( _dataSize % sizeof( uint32_t ) ) ) / sizeof( uint16_t );
				uint16_t* pData16    = (uint16_t*)pData;
				uint16_t* pSrcData16 = (uint16_t*)_data.get();
				uint16_t* pData16End = pData16 + dataSize16;
				do
				{
					*pData16++ = swapEndian<uint16_t>( *pSrcData16++ );
				}
				while( pData16 != pData16End );
				break;
			}
			// 16 bits doesn't need convertion
			case 16:
				break;
			// Need int swap
			default:
			{
				// Data have to be packed on uint32_t size to allow indianess fast
				// reinterpretation
				pData = new uint8_t[_dataSize + ( _dataSize % sizeof( uint32_t ) )];
				size_t dataSize32    = ( _dataSize + ( _dataSize % sizeof( uint32_t ) ) ) / sizeof( uint32_t );
				uint32_t* pData32    = (uint32_t*)pData;
				uint32_t* pSrcData32 = (uint32_t*)_data.get();
				uint32_t* pData32End = pData32 + dataSize32;
				do
				{
					*pData32++ = swapEndian<uint32_t>( *pSrcData32++ );
				}
				while( pData32 != pData32End );
				break;
			}
		}
	}
	return pData;
}

inline const size_t DpxImage::components() const
{
	switch( componentsType() )
	{
		case eCompTypeR8G8B8:
		case eCompTypeR10G10B10:
		case eCompTypeR12G12B12:
		case eCompTypeR16G16B16:
			return 3;
		case eCompTypeR8G8B8A8:
		case eCompTypeA8B8G8R8:
		case eCompTypeR10G10B10A10:
		case eCompTypeA10B10G10R10:
		case eCompTypeR12G12B12A12:
		case eCompTypeA12B12G12R12:
		case eCompTypeR16G16B16A16:
		case eCompTypeA16B16G16R16:
			return 4;
		default:
			break;
	}
	return 0;
}

const size_t DpxImage::dataSize() const
{
	size_t sz               = 0;
	boost::uint16_t packing = _header.packing();

	switch( componentsType() )
	{
		case eCompTypeR8G8B8:
			sz = sizeof( boost::uint8_t ) * 3 * width() * height();
			break;
		case eCompTypeR10G10B10:
			// Packing means that pixel are packed on bytes
			if( packing )
				sz = sizeof( boost::uint32_t ) * width() * height();
			// Unpacked means that pixels are bit aligned
			else
				sz = ( size_t ) std::ceil( ( 10 * 3 * width() * height() ) / 8.0f );
			break;
		case eCompTypeR8G8B8A8:
		case eCompTypeA8B8G8R8:
			sz = sizeof( boost::uint8_t ) * 4 * width() * height();
			break;
		case eCompTypeR12G12B12:
			sz = 6 * width() * height();
			break;
		case eCompTypeR16G16B16:
			sz = sizeof( uint16_t ) * 3 * width() * height();
			break;
		case eCompTypeR10G10B10A10:
		case eCompTypeA10B10G10R10:
			if( packing )
			{
				// This kind of packing is complex...
				int x = width() * height();
				if( x % 3 == 0 )
					return int(floor( x / 3.0f ) * 8 * 2);
				else if( x % 3 == 1 )
					return int(floor( x / 3.0f ) * 8 * 2 + 8);
				else if( x % 3 == 2 )
					return int(floor( x / 3.0f ) * 8 * 2 + 12);
			}
			else
				sz = 5 * width() * height();
			break;
		case eCompTypeR12G12B12A12:
		case eCompTypeA12B12G12R12:
			if( packing )
				sz = sizeof( boost::uint64_t ) * width() * height();
			else
				sz = ( size_t ) std::ceil( ( 12 * 4 * width() * height() ) / 8.0f );
			break;
		case eCompTypeR16G16B16A16:
		case eCompTypeA16B16G16R16:
			sz = sizeof( boost::uint64_t ) * width() * height();
			break;
		default:
			break;
	}
	return sz;
}

const DpxImage::EDPX_CompType DpxImage::componentsType() const
{
	EDPX_CompType type      = eCompTypeUnknown;
	unsigned int descriptor = _header.descriptor();
	unsigned int bitSize    = _header.bitSize();

	if( descriptor == 50 )
	{
		if( bitSize == 8 )
			type = eCompTypeR8G8B8;
		else if( bitSize == 10 )
			type = eCompTypeR10G10B10;
		else if( bitSize == 12 )
			type = eCompTypeR12G12B12;
		else if( bitSize == 16 )
			type = eCompTypeR16G16B16;
	}
	else if( descriptor == 51 )
	{
		if( bitSize == 8 )
			type = eCompTypeR8G8B8A8;
		else if( bitSize == 10 )
			type = eCompTypeR10G10B10A10;
		else if( bitSize == 12 )
			type = eCompTypeR12G12B12A12;
		else if( bitSize == 16 )
			type = eCompTypeR16G16B16A16;
	}
	else if( descriptor == 52 )
	{
		if( bitSize == 8 )
			type = eCompTypeA8B8G8R8;
		else if( bitSize == 10 )
			type = eCompTypeA10B10G10R10;
		else if( bitSize == 12 )
			type = eCompTypeA12B12G12R12;
		else if( bitSize == 16 )
			type = eCompTypeA16B16G16R16;
	}

	return type;
}

}
}
