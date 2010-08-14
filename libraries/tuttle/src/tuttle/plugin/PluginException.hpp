/**
 * This file helps managing exception for OfxPlugin.
 */

#ifndef _TUTTLE_PLUGIN_EXCEPTION_HPP
#define _TUTTLE_PLUGIN_EXCEPTION_HPP

#include <boost/lexical_cast.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_file_name.hpp>

#include <ofxCorePlugin.h>

#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

namespace tuttle {
namespace plugin {
namespace exception {

/**
 * @brief Standard tags you can use to add informations to your exceptions.
 *
 * @remark We keep lower camel case for tags,
 *	to keep a diffrence between tags and exception types
 */
/// @{
typedef boost::error_info<struct tag_message,std::string> message;
typedef boost::error_info<struct tag_ofxStatus,OfxStatus> ofxStatus;
/// @}


/// @brief Common exception for all plugins exceptions
class Common : virtual public std::exception, virtual public boost::exception {};

/// @brief Ofx standard errors
/// @{
class Failed : virtual public Common {};
class Fatal : virtual public Common {};
class Unknown : virtual public Common {};
class MissingHostFeature : virtual Common {};
class Unsupported : virtual public Common {};
class Exists : virtual public Common {};
class Format : virtual public Common {};
class Memory : virtual public Common {};
class BadHandle : virtual public Common {};
class BadIndex : virtual public Common {};
class Value : virtual public Common {};

/// @brief imageEffect specific errors
/// @{
class ImageFormat : virtual public Common {};
/// @}
/// @}


/// @brief Other plugins exceptions
/// @{
class BadConversion : virtual public Value {};
class ImageNotReady : virtual public Value {};
class ImageNotConnected : virtual public Value {};
class InputMismatch : virtual public Format {};
class InOutMismatch : virtual public Format {};
class BitDepthMismatch : virtual public ImageFormat {};
class WrongRowBytes : virtual public ImageFormat {};
class NullImageProvided : virtual public Value {};
class File : virtual public Value {};
/// @}

}
}
}

#endif
