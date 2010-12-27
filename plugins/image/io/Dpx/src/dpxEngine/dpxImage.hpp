#ifndef DPX_IMAGE_HPP
#define DPX_IMAGE_HPP

#include <tuttle/plugin/global.hpp>

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_array.hpp>
#include <cstring>
#include <cmath>
#include <iostream>

namespace tuttle {
namespace io {

namespace fs = boost::filesystem;

#define DPX_MAGIC           0x53445058 ///< dpx magic number is an interger corresponding to char[4] = "SDPX"
#define DPX_MAGIC_SWAP      0x58504453 ///< "XPDS"
///@todo tuttle: support for cineon file format
#define CIN_MAGIC           0x802A5FD7 ///< cineon magic number
#define UNDEF_U8            0xFF
#define UNDEF_U32           0xFFFFFFFF
#define UNDEF_S32           0x80000000
#define UNDEF_R32           0x7F800000
#define kDpxVersion         "V1.0"

struct FileInformation
{
	boost::uint32_t magic_num;         // magic number 0x53445058 (SDPX) or 0x58504453 (XPDS)
	boost::uint32_t offset;            // offset to image data in bytes
	char vers[8];                      // which header format version is being used (v1.0)
	boost::uint32_t file_size;         // file size in bytes
	boost::uint32_t ditto_key;         // read time short cut - 0 = same, 1 = new
	boost::uint32_t gen_hdr_size;      // generic header length in bytes
	boost::uint32_t ind_hdr_size;      // industry header length in bytes
	boost::uint32_t user_data_size;    // user-defined data length in bytes
	char file_name[100];    // image file name
	char create_time[24];   // file creation date "yyyy:mm:dd:hh:mm:ss:LTZ"
	char creator[100];      // file creator's name
	char project[200];      // project name
	char copyright[200];    // right to use or copyright info
	boost::uint32_t key;    // encryption ( FFFFFFFF = unencrypted )
	char reserved[104];     // reserved field TBD (need to pad)
};

std::ostream& operator<<( std::ostream& os, const FileInformation& v );


struct ImageInformation
{
	boost::uint16_t orientation;                        // image orientation
	boost::uint16_t element_number;                     // number of image elements
	boost::uint32_t pixelsPerLine;                      // or w value
	boost::uint32_t linesPerImageEle;                   // or h value, per element
	struct _image_element
	{
		boost::uint32_t data_sign;             // data sign (0 = unsigned, 1 = signed )
		// "Core set images are unsigned"
		boost::uint32_t ref_low_data;          // reference low data code value
		float ref_low_quantity;      // reference low quantity represented
		boost::uint32_t ref_high_data;         // reference high data code value
		float ref_high_quantity;     // reference high quantity represented
		boost::uint8_t descriptor;            // descriptor for image element
		boost::uint8_t transfer;              // transfer characteristics for element
		boost::uint8_t colorimetric;      // colorimetric specification for element
		boost::uint8_t bit_size;              // bit size for element
		boost::uint16_t packing;               // packing for element
		boost::uint16_t encoding;              // encoding for element
		boost::uint32_t data_offset;           // offset to data of element
		boost::uint32_t eol_padding;           // end of line padding used in element
		boost::uint32_t eo_image_padding;      // end of image padding used in element
		char description[32];               // description of element
	}
	image_element[8];                                     // NOTE THERE ARE EIGHT OF THESE
	boost::uint8_t reserved[52];                            // reserved for future use (padding)
};

std::ostream& operator<<( std::ostream& os, const ImageInformation::_image_element& v );
std::ostream& operator<<( std::ostream& os, const ImageInformation& v );

struct ImageOrientation
{
	boost::uint32_t x_offset;              // X offset
	boost::uint32_t y_offset;              // Y offset
	boost::uint32_t x_center;              // X center
	boost::uint32_t y_center;              // Y center
	boost::uint32_t x_orig_size;           // X original size
	boost::uint32_t y_orig_size;           // Y original size
	char file_name[100];                // source image file name
	char creation_time[24];             // source image creation date and time
	char input_dev[32];                 // input device name
	char input_serial[32];              // input device serial number
	boost::uint16_t border[4];             // border validity (XL, XR, YT, YB)
	boost::uint32_t pixel_aspect[2];       // pixel aspect ratio (H:V)
	boost::uint8_t reserved[28];          // reserved for future use (padding)
};

std::ostream& operator<<( std::ostream& os, const ImageOrientation& v );

struct MotionPictureFilm
{
	boost::uint8_t film_mfg_id;          // film manufacturer ID code (2 digits from film edge code)
	boost::uint8_t film_type;            // file type (2 digits from film edge code)
	boost::uint8_t offset;               // offset in perfs (2 digits from film edge code)
	boost::uint32_t prefix;              // prefix (6 digits from film edge code)
	boost::uint32_t count;               // count (4 digits from film edge code)
	char format[32];                     // format (i.e. academy)
	boost::uint32_t frame_position;      // frame position in sequence
	boost::uint32_t sequence_len;        // sequence length in frames
	boost::uint32_t held_count;          // held count (1 = default)
	float frame_rate;                    // frame rate of original in frames/sec
	float shutter_angle;                 // shutter angle of camera in degrees
	char frame_id[32];                   // frame identification (i.e. keyframe)
	char slate_info[100];                // slate information
	boost::uint8_t reserved[60];         // reserved for future use (padding)
};

std::ostream& operator<<( std::ostream& os, const MotionPictureFilm& v );

struct TelevisionHeader
{
	boost::uint32_t tim_code;            // SMPTE time code
	boost::uint32_t userBits;            // SMPTE user bits
	boost::uint8_t interlace;            // interlace ( 0 = noninterlaced, 1 = 2:1 interlace
	boost::uint8_t field_num;            // field number
	boost::uint8_t video_signal;         // video signal standard (table 4)
	boost::uint8_t unused;               // used for byte alignment only
	float hor_sample_rate;               // horizontal sampling rate in Hz
	float ver_sample_rate;               // vertical sampling rate in Hz
	float frame_rate;                    // temporal sampling rate or frame rate in Hz
	float time_offset;                   // time offset from sync to first pixel
	float gamma;                         // gamma value
	float black_level;                   // black level code value
	float black_gain;                    // black gain
	float break_point;                   // breakpoint
	float white_level;                   // reference white level code value
	float integration_times;             // integration time(s)
	boost::uint8_t reserved[76];         // reserved for future use (padding)
};


std::ostream& operator<<( std::ostream& os, const TelevisionHeader& v );

/// Class holding dpx informations
class DpxHeader
{
// Bad but easier

public:
	FileInformation _fileInfo;
	ImageInformation _imageInfo;
	ImageOrientation _imageOrientation;
	MotionPictureFilm _motionPicture;
	TelevisionHeader _television;

	DpxHeader()
	{
		// zeroify
		memset( &_fileInfo, 0, sizeof( FileInformation ) );
		memset( &_imageInfo, 0, sizeof( ImageInformation ) );
		memset( &_imageOrientation, 0, sizeof( ImageOrientation ) );
		memset( &_motionPicture, 0, sizeof( MotionPictureFilm ) );
		memset( &_television, 0, sizeof( TelevisionHeader ) );
		// Full size of 1664+384 = 2048 bytes per default
		_fileInfo.gen_hdr_size = 1664;
		_fileInfo.ind_hdr_size = 384;
		memcpy( _fileInfo.vers, kDpxVersion, strlen( kDpxVersion ) );
	}

	// Getters
	inline const bool            bigEndian() const                  { return _fileInfo.magic_num == DPX_MAGIC_SWAP; }
	inline const boost::uint32_t width() const                      { return _imageInfo.pixelsPerLine; }
	inline const boost::uint32_t height() const                     { return _imageInfo.linesPerImageEle; }
	inline const boost::uint16_t packing() const                    { return _imageInfo.image_element[0].packing; }
	inline const boost::uint8_t  bitSize() const                    { return _imageInfo.image_element[0].bit_size; }
	inline const boost::uint8_t  descriptor() const                 { return _imageInfo.image_element[0].descriptor; }
	inline const boost::uint32_t dataOffset() const                 { return _fileInfo.offset; }
	inline const boost::uint16_t elementNumber() const              { return _imageInfo.element_number; }
	inline const boost::uint32_t elementOffset( const int i ) const { return _imageInfo.image_element[i].data_offset; }

	// Setters
	inline void setBigEndian( const bool swap )
	{
		if( swap )
		{
			_fileInfo.magic_num = DPX_MAGIC_SWAP;
		}
		else
		{
			_fileInfo.magic_num = DPX_MAGIC;
		}
	}

	inline void setWidth( const boost::uint32_t pixelsPerLine )
	{
		_imageInfo.pixelsPerLine = pixelsPerLine;
	}

	inline void setHeight( const boost::uint32_t height )
	{
		_imageInfo.linesPerImageEle = height;
	}

	inline void setPacking( const boost::uint16_t packing, const int i )
	{
		assert( i < 8 && i > 0 );
		_imageInfo.image_element[i].packing = packing;
	}

	inline void setPacking( const boost::uint16_t packing )
	{
		for( int i = 0; i < 8; ++i )
			_imageInfo.image_element[i].packing = packing;
	}

	inline void setBitSize( const boost::uint8_t bitSize, const int i )
	{
		assert( i < 8 && i > 0 );
		_imageInfo.image_element[i].bit_size = bitSize;
	}

	inline void setBitSize( const boost::uint8_t bitSize )
	{
		for( int i = 0; i < 8; ++i )
			_imageInfo.image_element[i].bit_size = bitSize;
	}

	inline void setDescriptor( boost::uint8_t descriptor, const int i )
	{
		assert( i < 8 && i > 0 );
		_imageInfo.image_element[i].descriptor = descriptor;
	}

	inline void setDescriptor( boost::uint8_t descriptor )
	{
		for( int i = 0; i < 8; ++i )
			_imageInfo.image_element[i].descriptor = descriptor;
	}

	inline void setDataOffset( boost::uint32_t offset )
	{
		_fileInfo.offset = offset;
	}

	inline void setElementNumber( boost::uint16_t n )
	{
		_imageInfo.element_number = n;
	}

	inline void setElementOffset( boost::uint32_t offset, const int i )
	{
		_imageInfo.image_element[i].data_offset = offset;
	}

	void swapHeader();
};


std::ostream& operator<<( std::ostream& os, const DpxHeader& v );

class DpxImage
{
private:
	DpxHeader _header;
	size_t _dataSize;                               /// raw data size given by dataSize()
	boost::shared_array<boost::uint8_t> _data;      /// raw data
	boost::shared_array<boost::uint8_t> _indyData;  /// right endianness reinterpreted data

	void                                readHeader( fs::ifstream& f );
	bool                                isEndianReinterpNeeded() const;
	boost::shared_array<boost::uint8_t> reinterpretEndianness() const;
	size_t                              readDynamicHdrData( boost::uint8_t* dst, size_t maxLen, boost::uint8_t* buffer, size_t bufpos );

public:
	enum EDPX_CompType
	{
		eCompTypeUnknown,
		eCompTypeR8G8B8,    eCompTypeR8G8B8A8,     eCompTypeA8B8G8R8,
		eCompTypeR10G10B10, eCompTypeR10G10B10A10, eCompTypeA10B10G10R10,
		eCompTypeR12G12B12, eCompTypeR12G12B12A12, eCompTypeA12B12G12R12,
		eCompTypeR16G16B16, eCompTypeR16G16B16A16, eCompTypeA16B16G16R16
	};

	DpxImage();
	~DpxImage();

	void read( const fs::path& filename, bool reinterpretation = true );
	void readHeader( const fs::path& filename );
	void write( const fs::path& filename );
	void writeHeader( fs::ofstream& f );

	// Getters
	inline const boost::uint32_t width() const   { return _header.width(); }
	inline const boost::uint32_t height() const  { return _header.height(); }
	inline const boost::uint16_t packing() const { return _header.packing(); }
	inline const size_t          components() const;
	const EDPX_CompType          componentsType() const;
	const size_t                 dataSize() const;

	inline boost::uint8_t* rawData() const
	{
		return _data.get();
	}

	inline boost::uint8_t* data() const
	{
		return _indyData.get();
	}

	inline const boost::uint8_t compTypeToDescriptor( const EDPX_CompType type );

	// Setters
	inline const DpxHeader& getHeader() const { return _header; }
	inline void setHeader( const DpxHeader& header );
	inline void setComponentsType( EDPX_CompType );

	/**
	 * \brief Set raw data buffer, header must be set/read
	 * \param data	set data raw buffer
	 * \param reinterpretation	reinterpret endianness
	 */
	inline void setData( const boost::uint8_t* data, bool reinterpretation = true );
};

inline void DpxImage::setComponentsType( DpxImage::EDPX_CompType type )
{
	_header.setDescriptor( compTypeToDescriptor( type ) );
}

inline const boost::uint8_t DpxImage::compTypeToDescriptor( const EDPX_CompType type )
{
	boost::uint8_t ret = 53;

	switch( type )
	{
		case eCompTypeR8G8B8:
		case eCompTypeR10G10B10:
		case eCompTypeR12G12B12:
		case eCompTypeR16G16B16:
			ret = 50;
			break;
		case eCompTypeR8G8B8A8:
		case eCompTypeR10G10B10A10:
		case eCompTypeR12G12B12A12:
		case eCompTypeR16G16B16A16:
			ret = 51;
			break;
		case eCompTypeA8B8G8R8:
		case eCompTypeA10B10G10R10:
		case eCompTypeA12B12G12R12:
		case eCompTypeA16B16G16R16:
			ret = 52;
			break;
		default:
			// unknown format (53)
			break;
	}
	return ret;
}

inline void DpxImage::setHeader( const DpxHeader& header )
{
	memcpy( &_header, &header, sizeof( DpxHeader ) );
	_dataSize = dataSize();
	_header.setDataOffset( sizeof( DpxHeader ) );
	// Data have to be packed on uint32_t size to allow endianess fast
	// reinterpretation
	_data.reset( new boost::uint8_t[ _dataSize + ( _dataSize % sizeof( boost::uint32_t ) ) ] );
	_indyData = _data;
}

inline void DpxImage::setData( const boost::uint8_t* data, bool reinterpretation )
{
	memcpy( _data.get(), data, _dataSize );
	if( reinterpretation  && isEndianReinterpNeeded() )
	{
		_indyData = reinterpretEndianness();
	}
	else
	{
		_indyData = _data;
	}
}

}  // namespace io
}  // namespace tuttle

#endif // DPX_IMAGE_HPP
