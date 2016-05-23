#ifndef _FileGlobal_HPP
#define _FileGlobal_HPP

#include <tuttle/common/patterns/Singleton.hpp>
#include <tuttle/common/system/system.hpp>

#include <iostream>
#include <fstream>

/**
 * @brief Output datas in the same file from anywhere.
 **/
class FileGlobal : public Singleton<FileGlobal>
{
public:
    friend class Singleton<FileGlobal>;

private:
    FileGlobal()
        : _flux(NULL)
    {
    }
    ~FileGlobal() {}

public:
    std::ofstream _flux;

public:
    /// @param[in] fileName : file to create
    /// @brief File creation
    void openFile(std::string fileName)
    {
#ifndef __WINDOWS__
        _flux.open(fileName.c_str());
#else
        _flux.open(fileName.c_str(), std::ios::binary);
#endif
        if(!_flux)
        {
            std::cerr << "impossible de creer le fichier global : " << fileName << std::endl;
        }
    }

    /// @brief File close
    void closeFile() { _flux.close(); }
};

//______________________________________________________________________________
// Macros to output in global file

/**
 * @param[in] FILENAME : file name
 * @brief Change the filename for the output
 **/
#define FILEGLOBAL_FILENAME(FILENAME)                                                                                       \
    if(FileGlobal::instance()._flux)                                                                                        \
    {                                                                                                                       \
        FileGlobal::instance().closeFile();                                                                                 \
    }                                                                                                                       \
    FileGlobal::instance().openFile(FILENAME);

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief Output into the file define by FILEGLOBAL_FILENAME (if not specified, by default "out.log")
 **/
#define FILEGLOBAL(...)                                                                                                     \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << __VA_ARGS__ << std::endl;

#define FILEGLOBAL_VAR(a)                                                                                                   \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << #a << ": " << a << std::endl;

#define FILEGLOBAL_VAR2(a, b)                                                                                               \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << #a << ": " << a << ", " << #b << ": " << b << std::endl;

#define FILEGLOBAL_VAR3(a, b, c)                                                                                            \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << std::endl;

#define FILEGLOBAL_VAR4(a, b, c, d)                                                                                         \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << ", " << #d     \
                                 << ": " << d << std::endl;

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief Output into the file define by FILEGLOBAL_FILENAME (if not specified, by default "out.log")
 **/
#define FILEGLOBAL_INFO(...)                                                                                                \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << INFO << "\t" << __VA_ARGS__ << std::endl;

#define FILEGLOBAL_FUNCTION                                                                                                 \
    if(!FileGlobal::instance()._flux)                                                                                       \
    {                                                                                                                       \
        FileGlobal::instance().openFile("out.log");                                                                         \
    }                                                                                                                       \
    FileGlobal::instance()._flux << INFO << std::endl;

#endif
