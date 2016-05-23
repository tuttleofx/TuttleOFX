#ifndef _J2KREADER_HPP_
#define _J2KREADER_HPP_

#include "J2KCommon.hpp"

#include <tuttle/plugin/global.hpp>

#include <openjpeg.h>

#include <boost/scoped_array.hpp>
#include <boost/filesystem/fstream.hpp>

#include <string>

namespace tuttle
{
namespace io
{

class J2KReader
{
public:
    J2KReader();
    virtual ~J2KReader();

    void open(const std::string& filename);
    void decode(bool headeronly = false);
    void close();
    inline bool componentsConform(); ///< Check if components have the same properties
    // Getters
    inline const size_t components() const;                   ///< Get number of components
    inline const size_t width(const size_t nc = 0) const;     ///< Get width of nc component
    inline const size_t height(const size_t nc = 0) const;    ///< Get height of nc component
    inline const size_t precision(const size_t nc = 0) const; ///< Get precision of nc component
    inline const uint8_t* compData(const size_t nc) const;    ///< Get the nc component data
    inline bool imageReady() const;                           ///< Is image ready?
private:
    OpenJpegStuffs _openjpeg; ///< OpenJpeg 2000 structs
    uint8_t* _fileData;       ///< Image data
    std::ssize_t _dataLength; ///< Data length
};

inline bool J2KReader::imageReady() const
{
    return _openjpeg.image && _fileData;
}

inline bool J2KReader::componentsConform()
{
    size_t nc = components();
    if(nc > 0)
    {
        size_t i = 1;
        bool ret = true;
        size_t w = width();
        size_t h = height();
        size_t prec = precision();
        while(ret && i < nc)
        {
            if(width(i) != w || height(i) != h || precision(i) != prec)
            {
                ret = false;
            }
            ++i;
        }
        return ret;
    }
    return false;
}

inline const size_t J2KReader::components() const
{
    if(!_openjpeg.image)
    {
        return 0;
    }
    else
    {
        return _openjpeg.image->numcomps;
    }
}

inline const size_t J2KReader::width(const size_t nc /*= 0*/) const
{
    if(!_openjpeg.image)
    {
        return 0;
    }
    else
    {
        return _openjpeg.image->x1 - _openjpeg.image->x0;
    }
}

inline const size_t J2KReader::height(const size_t nc /*= 0*/) const
{
    if(!_openjpeg.image)
    {
        return 0;
    }
    else
    {
        return _openjpeg.image->y1 - _openjpeg.image->y0;
    }
}

inline const size_t J2KReader::precision(const size_t nc /*= 0*/) const
{
    if(!_openjpeg.image)
    {
        return 0;
    }
    else
    {
        assert(nc < components());
        return _openjpeg.image->comps[nc].prec;
    }
}

/// @warning data are int packed!
inline const uint8_t* J2KReader::compData(const size_t nc) const
{
    if(!_openjpeg.image)
    {
        return NULL;
    }
    else
    {
        assert(nc < components());
        return (uint8_t*)_openjpeg.image->comps[nc].data;
    }
}
}
}

#endif
