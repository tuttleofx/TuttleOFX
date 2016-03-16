#ifndef _J2KWRITER_HPP
#define _J2KWRITER_HPP

#include "J2KCommon.hpp"

#include <openjpeg.h>

#include <boost/filesystem/fstream.hpp>

#include <string>

namespace tuttle
{
namespace io
{

class J2KWriter
{
public:
    J2KWriter();
    virtual ~J2KWriter();
    void open(const std::string& filename, const size_t w, const size_t h, const size_t nc, const size_t dprecision);
    void encode(const uint8_t* data, const size_t sprecision);
    void close();

    // Setters
    inline void setLossless(bool lossless);
    inline void setPrecision(const size_t prec);
    inline void setCinemaMode(const OPJ_CINEMA_MODE cinemaMode);

private:
    void cinemaSetupParameters();
    void cinemaSetupEncoder();
    int initialize4Kpocs(opj_poc_t* POC, int numres);

private:
    OpenJpegStuffs _openjpeg; ///< OpenJpeg 2000 structs
    size_t _width;
    size_t _height;
    size_t _components;
    size_t _precision;
    bool _lossless;
    OPJ_CINEMA_MODE _cinemaMode;
    boost::filesystem::ofstream _outFile;
    opj_cio_t* _cio;
};

// Setters

inline void J2KWriter::setLossless(bool lossless)
{
    _lossless = lossless;
}

inline void J2KWriter::setPrecision(const size_t prec)
{
    _precision = prec;
}

inline void J2KWriter::setCinemaMode(const OPJ_CINEMA_MODE cinemaMode)
{
    _cinemaMode = cinemaMode;
}
}
}

#endif
