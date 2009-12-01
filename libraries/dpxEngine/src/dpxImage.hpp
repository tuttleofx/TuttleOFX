/**
 * dpxImage.hpp
 *
 *  Created on: 10 nov. 2009
 *      Author: Nicolas Rondaud & Eloi Du Bois
 */

#ifndef DPX_IMAGE_HPP
#define DPX_IMAGE_HPP

#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <cmath>

namespace tuttle {
namespace io {

namespace fs = boost::filesystem;

#define DPX_MAGIC           0x53445058
#define DPX_MAGIC_SWAP      0x58504453
#define UNDEF_U8            0xFF
#define UNDEF_U32           0xFFFFFFFF
#define UNDEF_S32           0x80000000
#define UNDEF_R32           0x7F800000

struct FileInformation
{
	boost::uint32_t magic_num;         // magic number 0x53445058 (SDPX) or 0x58504453 (XPDS)
	boost::uint32_t offset;            // offset to image data in bytes
	char vers[8];           // which header format version is being used (v1.0)
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
	boost::uint32_t key;               // encryption ( FFFFFFFF = unencrypted )
	char Reserved[104];     // reserved field TBD (need to pad)
};

struct ImageInformation
{
	boost::uint16_t orientation;                        // image orientation
	boost::uint16_t element_number;                     // number of image elements
	boost::uint32_t pixelsPerLine;                      // or x value
	boost::uint32_t linesPerImageEle;                   // or y value, per element
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

struct MotionPictureFilm
{
	boost::uint8_t film_mfg_id;           // film manufacturer ID code (2 digits from film edge code)
	boost::uint8_t film_type;             // file type (2 digits from film edge code)
	boost::uint8_t offset;                // offset in perfs (2 digits from film edge code)
	boost::uint32_t prefix;                // prefix (6 digits from film edge code)
	boost::uint32_t count;                 // count (4 digits from film edge code)
	char format[32];                    // format (i.e. academy)
	boost::uint32_t frame_position;        // frame position in sequence
	boost::uint32_t sequence_len;          // sequence length in frames
	boost::uint32_t held_count;            // held count (1 = default)
	float frame_rate;                    // frame rate of original in frames/sec
	float shutter_angle;         // shutter angle of camera in degrees
	char frame_id[32];          // frame identification (i.e. keyframe)
	char slate_info[100];       // slate information
	boost::uint8_t reserved[60];          // reserved for future use (padding)
};

struct TelevisionHeader
{
	boost::uint32_t tim_code;              // SMPTE time code
	boost::uint32_t userBits;              // SMPTE user bits
	boost::uint8_t interlace;             // interlace ( 0 = noninterlaced, 1 = 2:1 interlace
	boost::uint8_t field_num;             // field number
	boost::uint8_t video_signal;          // video signal standard (table 4)
	boost::uint8_t unused;                // used for byte alignment only
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
	boost::uint8_t reserved[76];          // reserved for future use (padding)
};

typedef struct
{
	FileInformation _fileInfo;
	ImageInformation _imageInfo;
	ImageOrientation _imageOrientation;
	MotionPictureFilm _motionPicture;
	TelevisionHeader _television;
} DpxHeader;

class DpxImage
{
private:
	bool _bigEndian;
	DpxHeader _header;
	boost::uint8_t* _data;

	void readHeader( fs::ifstream& f );

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
	void read( const fs::path& filename );
	void readHeader( const fs::path& filename );
	void write( const fs::path& filename );

	inline const boost::uint32_t& width() const   { return _header._imageInfo.pixelsPerLine; }
	inline const boost::uint32_t& height() const  { return _header._imageInfo.linesPerImageEle; }
	inline const boost::uint16_t  packing() const { return _header._imageInfo.image_element[0].packing; }

	const size_t components() const
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

	const size_t dataSize() const
	{
		size_t sz               = 0;
		boost::uint16_t packing = _header._imageInfo.image_element[0].packing;

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
			case eCompTypeR16G16B16:
			case eCompTypeR10G10B10A10:
			case eCompTypeA10B10G10R10:
				sz = 5 * width() * height();
				break;
			case eCompTypeR12G12B12A12:
			case eCompTypeA12B12G12R12:
			case eCompTypeR16G16B16A16:
			case eCompTypeA16B16G16R16:
				sz = sizeof( boost::uint64_t ) * width() * height();
				break;
			default:
				break;
		}
		return sz;
	}

	const EDPX_CompType componentsType() const
	{
		EDPX_CompType type      = eCompTypeUnknown;
		unsigned int descriptor = _header._imageInfo.image_element[0].descriptor;
		unsigned int bitSize    = _header._imageInfo.image_element[0].bit_size;

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

	inline const boost::uint8_t* data() const
	{
		return _data;
	}

	/// NEED TO BE DELETED BY CALLER
	const boost::uint16_t* data16() const;

};

}  // namespace io
}  // namespace tuttle

#endif // DPX_IMAGE_HPP
