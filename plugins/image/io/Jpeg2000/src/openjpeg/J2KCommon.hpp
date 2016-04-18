#ifndef _J2KCOMMON_HPP_
#define _J2KCOMMON_HPP_

#include <openjpeg.h>

// Magic number based on some files found on various disks.
// Specs not availiable at date of writing
#define MAYBE_MAGIC 0x51ff4fff
#define MAYBE_REV_MAGIC 0xff4fff51
// Magic number should be:
// 12 byte string: X'0000 000C 6A50 1A1A 0D0A 870A'

#define CINEMA_24_CS 1302083 /*Codestream length for 24fps*/
#define CINEMA_48_CS 651041  /*Codestream length for 48fps*/
#define COMP_24_CS 1041666   /*Maximum size per color component for 2K & 4K @ 24fps*/
#define COMP_48_CS 520833    /*Maximum size per color component for 2K @ 48fps*/

#define J2K_CFMT 0
#define JP2_CFMT 1
#define JPT_CFMT 2

// On tente de trouver un magic number. Based on some files found on various disks. Specs not availiable at date of writing
#define MAYBE_MAGIC 0x51ff4fff
#define MAYBE_REV_MAGIC 0xff4fff51

namespace tuttle
{
namespace io
{

struct OpenJpegStuffs
{
    opj_image_t* image;
    opj_cparameters_t parameters; /* compression parameters */
    opj_event_mgr_t event_mgr;    /* event manager */
};
}
}

#endif
