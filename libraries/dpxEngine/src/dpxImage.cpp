#include "dpxImage.hpp"
#include <dpxUtils.hpp>

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/endian.hpp>

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
inline T highbit(T& t)
{
	return t = (((T)(-1)) >> 1) + 1;
}

template < typename T >
std::ostream& bin(T& value, std::ostream &o)
{
	for ( T bit = highbit(bit); bit; bit >>= 1 )
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
: _bigEndian( false )
{
}

DpxImage::~DpxImage()
{
}

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
	// read and throws away characters until 'offset' characters have been read
	f.ignore( _header.dataOffset() - f.tellg() );
	// Data have to be packed on uint32_t size to allow indianess fast
	// reinterpretation
	_data.reset(new uint8_t[ _dataSize + ( _dataSize % sizeof(uint32_t) ) ]);
	// read data
	if( !f.read( reinterpret_cast<char*>( _data.get() ), _dataSize ) )
	{
		std::ostringstream msg;
		msg << "Unable to read data ";
		msg << "( " << filename << " )" ;
		throw std::exception();
	}
	if ( reinterpretation )
		_indyData.reset(reinterpretEndianness());
	else
		_indyData = _data;
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
	ImageInformation* imgif;
	ImageOrientation* imgor;
	MotionPictureFilm* mp;
	TelevisionHeader* tv;

	// reads beginning data from _header (we need to get the
	// header size given in file)...
	f.seekg( 0, std::ios::beg );
	if( !f.read( reinterpret_cast<char*>( &_header ), 32 ) )
	{
		std::ostringstream msg;
		msg << "Unable to read header...";
		std::cerr << "Unable to read dpx header..." << std::endl;
		throw std::exception();
	}

	// ...file information
	gen            = &( _header._fileInfo );
	gen->magic_num =   gen->magic_num;
	_bigEndian     = ( gen->magic_num == DPX_MAGIC_SWAP );
	if( _bigEndian )
	{
		//
		// "BIG ENDIAN"
		//
		gen->magic_num    =   swapEndian<unsigned long>( gen->magic_num );
		gen->offset       =   swapEndian<unsigned long>( gen->offset );
		gen->file_size    =   swapEndian<unsigned long>( gen->file_size );
		gen->ditto_key    =   swapEndian<unsigned long>( 1 );
		gen->gen_hdr_size =   swapEndian<unsigned long>( gen->gen_hdr_size );
		gen->ind_hdr_size =   swapEndian<unsigned long>( gen->ind_hdr_size );
	}
	size_t hdrSize = ( gen->gen_hdr_size > 0 ? gen->gen_hdr_size : sizeof( DpxHeader ) ) - 32;

	// reads data from _header...
	if( !f.read( reinterpret_cast<char*>( &_header ) + 32, hdrSize ) )
	{
		std::ostringstream msg;
		msg << "DPX: Unable to read header...";
		std::cerr << msg << std::endl;
		throw std::exception();
	}

	if( _bigEndian )
	{
		//
		// "BIG ENDIAN"
		//
		gen->user_data_size =   swapEndian<unsigned long>( gen->user_data_size );
		gen->key            =   swapEndian<unsigned long>( gen->key );

		// ...image information
		imgif                   = &( _header._imageInfo );
		imgif->orientation      = swapEndian<short>( imgif->orientation );
		imgif->element_number   = swapEndian<short>( imgif->element_number );
		imgif->pixelsPerLine    = swapEndian<unsigned long>( imgif->pixelsPerLine );
		imgif->linesPerImageEle = swapEndian<unsigned long>( imgif->linesPerImageEle );
		for( int i = 0; i < 8; i++ )
		{
			imgif->image_element[i].data_sign         = swapEndian<unsigned long>( imgif->image_element[i].data_sign );
			imgif->image_element[i].ref_low_data      = swapEndian<unsigned long>( imgif->image_element[i].ref_low_data );
			imgif->image_element[i].ref_low_quantity  = swapEndian<float>( imgif->image_element[i].ref_low_quantity );
			imgif->image_element[i].ref_high_data     = swapEndian<unsigned long>( imgif->image_element[i].ref_high_data );
			imgif->image_element[i].ref_high_quantity = swapEndian<float>( imgif->image_element[i].ref_high_quantity );
			imgif->image_element[i].packing           = swapEndian<short>( imgif->image_element[i].packing );
			imgif->image_element[i].encoding          = swapEndian<short>( imgif->image_element[i].encoding );
			imgif->image_element[i].data_offset       = swapEndian<unsigned long>( imgif->image_element[i].data_offset );
			imgif->image_element[i].eol_padding       = swapEndian<unsigned long>( imgif->image_element[i].eol_padding );
			imgif->image_element[i].eo_image_padding  = swapEndian<unsigned long>( imgif->image_element[i].eo_image_padding );
		}

		// ...file orientation
		imgor                  = &( _header._imageOrientation );
		imgor->x_offset        =   swapEndian<unsigned long>( imgor->x_offset );
		imgor->y_offset        =   swapEndian<unsigned long>( imgor->y_offset );
		imgor->x_center        =   swapEndian<unsigned long>( imgor->x_center );
		imgor->y_center        =   swapEndian<unsigned long>( imgor->y_center );
		imgor->x_orig_size     =   swapEndian<unsigned long>( imgor->x_orig_size );
		imgor->y_orig_size     =   swapEndian<unsigned long>( imgor->y_orig_size );
		imgor->border[0]       =   swapEndian<short>( imgor->border[0] );
		imgor->border[1]       =   swapEndian<short>( imgor->border[1] );
		imgor->border[2]       =   swapEndian<short>( imgor->border[2] );
		imgor->border[3]       =   swapEndian<short>( imgor->border[3] );
		imgor->pixel_aspect[0] =   swapEndian<unsigned long>( imgor->pixel_aspect[0] );
		imgor->pixel_aspect[1] =   swapEndian<unsigned long>( imgor->pixel_aspect[1] );

		// ...motion picture film _header
		mp                 =   &( _header._motionPicture );
		mp->prefix         =   swapEndian<unsigned long>( mp->prefix );
		mp->count          =   swapEndian<unsigned long>( mp->count );
		mp->frame_position =   swapEndian<unsigned long>( mp->frame_position );
		mp->sequence_len   =   swapEndian<unsigned long>( mp->sequence_len );
		mp->held_count     =   swapEndian<unsigned long>( mp->held_count );
		mp->frame_rate     =   swapEndian<float>( mp->frame_rate );
		mp->shutter_angle  =   swapEndian<float>( mp->shutter_angle );

		// ...television _header
		tv                    =   &( _header._television );
		tv->tim_code          =   swapEndian<unsigned long>( tv->tim_code );
		tv->userBits          =   swapEndian<unsigned long>( tv->userBits );
		tv->hor_sample_rate   =   swapEndian<float>( tv->hor_sample_rate );
		tv->ver_sample_rate   =   swapEndian<float>( tv->ver_sample_rate );
		tv->frame_rate        =   swapEndian<float>( tv->frame_rate );
		tv->time_offset       =   swapEndian<float>( tv->time_offset );
		tv->gamma             =   swapEndian<float>( tv->gamma );
		tv->black_level       =   swapEndian<float>( tv->black_level );
		tv->black_gain        =   swapEndian<float>( tv->black_gain );
		tv->break_point       =   swapEndian<float>( tv->break_point );
		tv->white_level       =   swapEndian<float>( tv->white_level );
		tv->integration_times =   swapEndian<float>( tv->integration_times );
	}

	if( _header._imageInfo.orientation > 1 )
	{
		std::ostringstream msg;
		msg << "DPX: bad orientation value";
		std::cerr << msg << std::endl;
		throw std::exception();
	}

	uint8_t bitSize = _header._imageInfo.image_element[0].bit_size;
	if( bitSize != 8 && bitSize != 10 && bitSize != 12 && bitSize != 16)
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

void DpxImage::write( const path& filename ) {
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
	_header.setDataOffset(2048);
	writeHeader( f );
	// Pad with zeros untill we get at the right offset
	std::vector<uint8_t> zeros(_header.dataOffset() - f.tellp());
	if (zeros.size())
		if( !f.write( reinterpret_cast<char*>( &zeros[0] ), zeros.size() ) ) {
			throw std::exception();
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

void DpxImage::writeHeader( ofstream& f ) {
	// write data
	if( !f.write( reinterpret_cast<char*>( &_header._fileInfo ), sizeof(FileInformation) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (FileInformation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &_header._imageInfo ), sizeof(ImageInformation) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (ImageInformation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &_header._imageOrientation ), sizeof(ImageOrientation) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (ImageOrientation)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &_header._motionPicture ), sizeof(MotionPictureFilm) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (MotionPictureFilm)" ;
		throw std::exception();
	}
	if( !f.write( reinterpret_cast<char*>( &_header._television ), sizeof(TelevisionHeader) ) )
	{
		std::ostringstream msg;
		msg << "DpxImage::Unable to write data (TelevisionHeader)" ;
		throw std::exception();
	}
}

uint8_t* DpxImage::reinterpretEndianness() const
{
	// Do we need reinterpretation ?
	uint8_t* pData = _data.get();
	if (_bigEndian && BOOST_BYTE_ORDER == 1234) {
		switch (_header.bitSize()) {
			// 8 bits doesn't need convertion
			case 8:
				break;
			// Need short swap
			case 12:
			{
				// Data have to be packed on uint32_t size to allow indianess fast
				// reinterpretation
				pData    = new uint8_t[_dataSize + (_dataSize % sizeof(uint32_t))];
				size_t dataSize16 = (_dataSize + (_dataSize % sizeof(uint32_t))) / sizeof(uint16_t);
				uint16_t *pData16 = (uint16_t*)pData;
				uint16_t *pSrcData16 = (uint16_t*)_data.get();
				uint16_t *pData16End = pData16 + dataSize16;
				do {
					*pData16++ = swapEndian<uint16_t>(*pSrcData16++);
				} while(pData16 != pData16End);
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
				pData    = new uint8_t[_dataSize + (_dataSize % sizeof(uint32_t))];
				size_t dataSize32 = (_dataSize + (_dataSize % sizeof(uint32_t))) / sizeof(uint32_t);
				uint32_t *pData32 = (uint32_t*)pData;
				uint32_t *pSrcData32 = (uint32_t*)_data.get();
				uint32_t *pData32End = pData32 + dataSize32;
				do {
					*pData32++ = swapEndian<uint32_t>(*pSrcData32++);
				} while(pData32 != pData32End);
				break;
			}
		}
	}
	return pData;
}

inline const size_t DpxImage::components() const	{
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
			sz = sizeof(uint16_t) * 3 * width() * height();
			break;
		case eCompTypeR10G10B10A10:
		case eCompTypeA10B10G10R10:
			if( packing ) {
				// This kind of packing is complex...
				int x = width() * height();
				if (x%3 == 0)
					return int(floor(x / 3.0f)*8*2);
				else if (x%3 == 1)
					return int(floor(x / 3.0f)*8*2+8);
				else if (x%3 == 2)
					return int(floor(x / 3.0f)*8*2+12);
			} else
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
