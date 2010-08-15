#ifndef _TUTTLE_PLUGIN_EXCEPTION_HPP_
#define _TUTTLE_PLUGIN_EXCEPTION_HPP_

#include <boost/lexical_cast.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_file_name.hpp>

#include <ofxCorePlugin.h>
#include <ofxImageEffect.h>

#include <sstream>
#include <string>
#include <stdexcept>

namespace tuttle {
namespace plugin {
namespace exception {

/**
 * @brief Standard tags you can use to add informations to your exceptions.
 *
 * @remark We use lower camel case for tags,
 *	to keep a difference between tags and exceptions
 */
/// @{
typedef boost::error_info<struct tag_message,std::string> message;
typedef boost::error_info<struct tag_ofxStatus,::OfxStatus> ofxStatus;
typedef boost::errinfo_file_name filename;
/// @}


/// @brief Common exception for all tuttle plugin exceptions
struct Common : virtual public std::exception, virtual public boost::exception {};

/// @brief Ofx standard errors
/// @{

/** @brief Status error code for a failed operation */
struct Failed : virtual public Common
{
	Failed()
	{
		*this << ofxStatus( kOfxStatFailed );
	}
};

/**
 * @brief Status error code for a fatal error
 *
 * Only returned in the case where the plug-in or host cannot continue to function and needs to be restarted.
 */
struct Fatal : virtual public Common
{
	Fatal()
	{
		*this << ofxStatus( kOfxStatErrFatal );
	}
};

/** @brief Status error code for an operation on or request for an unknown object */
struct Unknown : virtual public Common
{
	Unknown()
	{
		*this << ofxStatus( kOfxStatErrUnknown );
	}
};

/**
 * @brief Status error code returned by plug-ins when they are missing host functionality, either an API or some optional functionality (eg: custom params).
 *
 * Plug-Ins returning this should post an appropriate error message stating what they are missing.
 */
struct MissingHostFeature : virtual Common
{
	MissingHostFeature()
	{
		*this << ofxStatus( kOfxStatErrMissingHostFeature );
	}
};

/** @brief Status error code for an unsupported feature/operation */
struct Unsupported : virtual public Common
{
	Unsupported()
	{
		*this << ofxStatus( kOfxStatErrUnsupported );
	}
};

/** @brief Status error code for an operation attempting to create something that exists */
struct Exists : virtual public Common
{
	Exists()
	{
		*this << ofxStatus( kOfxStatErrExists );
	}
};

/** @brief Status error code for an incorrect format */
struct Format : virtual public Common
{
	Format()
	{
		*this << ofxStatus( kOfxStatErrFormat );
	}
};

/** @brief Status error code indicating that something failed due to memory shortage */
struct Memory : virtual public Common
{
	Memory()
	{
		*this << ofxStatus( kOfxStatErrMemory );
	}
};

/** @brief Status error code for an operation on a bad handle */
struct BadHandle : virtual public Common
{
	BadHandle()
	{
		*this << ofxStatus( kOfxStatErrBadHandle );
	}
};

/** @brief Status error code indicating that a given index was invalid or unavailable */
struct BadIndex : virtual public Common
{
	BadIndex()
	{
		*this << ofxStatus( kOfxStatErrBadIndex );
	}
};

/** @brief Status error code indicating that something failed due an illegal value */
struct Value : virtual public Common
{
	Value()
	{
		*this << ofxStatus( kOfxStatErrValue );
	}
};

/// @brief imageEffect specific errors
/// @{

/** @brief Error code for incorrect image formats */
struct ImageFormat : virtual public Common
{
	ImageFormat()
	{
		*this << ofxStatus( kOfxStatErrImageFormat );
	}
};
/// @}
/// @}


/// @brief Other plugins exceptions
/// @{
/** @brief Unknown error inside a conversion. */
struct BadConversion : virtual public Value {};
/** @brief The host launch a process, but the input clips are not filled (eg. NULL buffer pointer). The error comes from host. */
struct ImageNotReady : virtual public Value {};
/**
 * @brief A non optional input clip in not connected.
 * Normally the host should not launch the render in this case.
 * The error comes from host.
 */
struct ImageNotConnected : virtual public Value {};
/**
 * @brief Input property don't satisfy descriptor requirements.
 * The error comes from host.
 */
struct InputMismatch : virtual public Format {};
/**
 * @brief Input and output properties mismatch.
 * The error comes from host.
 */
struct InOutMismatch : virtual public Format {};
/**
 * @brief Input(s) and enventually output properties mismatch.
 * The error comes from host.
 */
struct BitDepthMismatch : virtual public ImageFormat {};
/**
 * @brief Image raw bytes not valid.
 * The error comes from host.
 */
struct WrongRowBytes : virtual public ImageFormat {};
/**
 * @brief File manipulation error.
 * eg. read only, file doesn't exists, etc.
 */
struct File : virtual public Value {};
/// @}

}
}
}

#endif
