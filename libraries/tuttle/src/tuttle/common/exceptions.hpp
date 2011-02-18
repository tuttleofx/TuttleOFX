#ifndef _TUTTLE_COMMON_EXCEPTION_HPP_
#define _TUTTLE_COMMON_EXCEPTION_HPP_

#include "boost_error_info_sstream.hpp"

#include <tuttle/common/ofx/core.hpp>
#include <tuttle/common/ofx/utilities.hpp>
#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/get_error_info.hpp>

#include <ofxCorePlugin.h>
#include <ofxImageEffect.h>

#include <sstream>
#include <string>
#include <stdexcept>

#define TUTTLE_COUT_EXCEPTION( e )  \
    TUTTLE_CERR( "Exception:" << \
    ::std::endl << TUTTLE_INFOS << \
    ::std::endl << "\t" << ::boost::diagnostic_information( e ) )

#define TUTTLE_COUT_CURRENT_EXCEPTION  \
    TUTTLE_CERR( "Exception:" << \
    ::std::endl << TUTTLE_INFOS << \
    ::std::endl << "\t" << ::boost::current_exception_diagnostic_information() )

namespace OFX {
struct tag_ofxStatus;
typedef ::boost::error_info<OFX::tag_ofxStatus, ::OfxStatus> ofxStatus;
inline std::string to_string( const ofxStatus& e ) { return ::tuttle::ofx::mapStatusToString( e.value() ); }
}

namespace tuttle {

/**
 * @brief To add quotes around a string.
 * @example quotes("toto") -> "\"toto\""
 */
inline std::string quotes( const std::string& s )
{
	return "\"" + s + "\"";
}

namespace exception {

#ifndef SWIG
/**
 * @brief Like a stringstream but using "operator+" instead of "operator<<".
 * Use a stringstream internally.
 */

/**
 * @brief Standard tags you can use to add informations to your exceptions.
 *
 * @remark We use lower camel case for tags,
 *	to keep a difference between tags and exceptions.
 */
/// @{
/**
 * @brief If you catch an error at the top level, you can print this information to the user.
 * @remark User information.
 */
typedef ::boost::error_info<struct tag_userMessage, ::boost::error_info_sstream> user;

/**
 * @brief This is detailed informations for developpers.
 * Not always a real human readable message :)
 * @remark Dev information.
 */
//typedef ::boost::error_info<struct tag_message,std::string> dev;
typedef ::boost::error_info<struct tag_devMessage, ::boost::error_info_sstream> dev;
//typedef ::boost::error_info_sstream<struct tag_message> dev;

/**
 * @brief The ofx error status code.
 * @remark Dev information.
 */
typedef ::OFX::ofxStatus ofxStatus;

/**
 * @brief The ofx context name.
 * Each plugin can be instanciated in differents contexts (depending on the declaration of supported contexts).
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_ofxContext, ::std::string> ofxContext;
/**
 * @brief The ofx api string identification.
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_ofxApi, ::std::string> ofxApi;
/**
 * @brief Plugin string identifier.
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_pluginIdentifier, ::std::string> pluginIdentifier;
/**
 * @brief Plugin name.
 * @remark User information.
 */
typedef ::boost::error_info<struct tag_pluginName, ::std::string> pluginName;
/**
 * @brief Node name.
 * @remark User information.
 */
typedef ::boost::error_info<struct tag_nodeName, ::std::string> nodeName;
/**
 * @brief Time.
 * @remark Dev or user information.
 */
typedef ::boost::error_info<struct tag_time, OfxTime> time;
/**
 * @brief Problem with a file.
 * @remark User information.
 */
typedef ::boost::errinfo_file_name filename;
/// @}
#endif

/** @brief Common exception for all tuttle plugin exceptions */
struct Common : virtual public ::std::exception
	, virtual public ::boost::exception {};

/// @brief Ofx standard errors
/// @{

/**
 * @brief You have to specify the exception::ofxStatus(kOfxStatXXX) yourself.
 * When you call a base level function (C API) which returns an ofxStatus, you can use this exception and fill it with the returned value using ofxStatus tag.
 */
struct OfxCustom : virtual public Common
{
	OfxCustom( const OfxStatus status )
	{
		*this << ofxStatus( status );
	}
};

/** @brief Status error code for a failed operation */
struct Failed : virtual public Common
{
	Failed()
	{
		* this << ofxStatus( kOfxStatFailed );
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
		* this << ofxStatus( kOfxStatErrFatal );
	}

};

/** @brief Status error code for an operation on or request for an unknown object */
struct Unknown : virtual public Common
{
	Unknown()
	{
		* this << ofxStatus( kOfxStatErrUnknown );
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
		* this << ofxStatus( kOfxStatErrMissingHostFeature );
	}
	MissingHostFeature( const std::string& feature )
	{
		* this << ofxStatus( kOfxStatErrMissingHostFeature );
		* this << user() + "Missing feature: " + quotes(feature);
	}
};

/** @brief Status error code for an unsupported feature/operation */
struct Unsupported : virtual public Common
{
	Unsupported()
	{
		* this << ofxStatus( kOfxStatErrUnsupported );
	}

};

/** @brief Status error code for an operation attempting to create something that exists */
struct Exists : virtual public Common
{
	Exists()
	{
		* this << ofxStatus( kOfxStatErrExists );
	}

};

/** @brief Status error code for an incorrect format */
struct Format : virtual public Common
{
	Format()
	{
		* this << ofxStatus( kOfxStatErrFormat );
	}

};

/** @brief Status error code indicating that something failed due to memory shortage */
struct Memory : virtual public Common
{
	Memory()
	{
		* this << ofxStatus( kOfxStatErrMemory );
	}

};

/** @brief Status error code for an operation on a bad handle */
struct BadHandle : virtual public Common
{
	BadHandle()
	{
		* this << ofxStatus( kOfxStatErrBadHandle );
	}

};

/** @brief Status error code indicating that a given index was invalid or unavailable */
struct BadIndex : virtual public Common
{
	BadIndex()
	{
		* this << ofxStatus( kOfxStatErrBadIndex );
	}

};

/** @brief Status error code indicating that something failed due an illegal value */
struct Value : virtual public Common
{
	Value()
	{
		* this << ofxStatus( kOfxStatErrValue );
	}

};

/// @brief imageEffect specific errors
/// @{

/** @brief Error code for incorrect image formats */
struct ImageFormat : virtual public Common
{
	ImageFormat()
	{
		* this << ofxStatus( kOfxStatErrImageFormat );
	}

};
/// @}
/// @}

/// @brief Other exceptions
/// @{

/**
 * @brief The class serves as the base class for all exceptions thrown to report errors presumably detectable before the program executes, such as violations of logical preconditions (cf. std::logic_error).
 * @remark With this exception, you normally have a "user" tag message.
 */
struct Logic : virtual public Value {};

/**
 * @brief Something that should never appends.
 *        These exceptions may be replaced by assertions,
 *        but we prefer to keep a runtime check even in release (for the moment).
 * @remark With this exception, you should have a "dev" tag message.
 */
struct Bug : virtual public Value {};

/** @brief Unknown error inside a conversion. */
struct BadConversion : virtual public Value {};

/** @brief Error with a NULL image buffer.
 * * plugin: The host launch a process, but the input clips are not filled (eg. NULL buffer pointer).
 *           The error comes from host.
 * * host:   Error with memory cache or memory pool.
 */
struct ImageNotReady : virtual public Value {};

/**
 * @brief A non optional input clip in not connected.
 * * plugin: Normally the host should not launch the render in this case.
 *           The error comes from host.
 * * host: We can't launch the render in this case.
 */
struct ImageNotConnected : virtual public Value {};

/**
 * @brief Input property don't satisfy descriptor requirements.
 * * plugin:  The error comes from host.
 * * host: We can't launch the render in this case.
 */
struct InputMismatch : virtual public Format {};

/**
 * @brief Input and output properties mismatch.
 * * plugin:  The error comes from host.
 * * host: We can't launch the render in this case.
 */
struct InOutMismatch : virtual public Format {};

/**
 * @brief Input(s) and enventually output properties mismatch.
 * * plugin:  The error comes from host.
 * * host: We can't launch the render in this case.
 */
struct BitDepthMismatch : virtual public ImageFormat {};

/**
 * @brief Image raw bytes not valid.
 * * plugin:  The error comes from host.
 * * host: We can't launch the render in this case.
 */
struct WrongRowBytes : virtual public ImageFormat {};

/** @brief Status error code indicating that something failed due an illegal data */
struct Data : virtual public Value {};

/**
 * @brief File manipulation error.
 * eg. read only, file doesn't exists, etc.
 */
struct File : virtual public Value
{
	File()
	{}
	File( const std::string path )
	{
		*this << filename(path);
	}
};

/**
 * @brief File doesn't exists.
 */
struct FileNotExist : virtual public File
{
	FileNotExist()
	{}
	FileNotExist( const std::string path )
	{
		*this << filename(path);
	}
};

/**
 * @brief Directory doesn't exists.
 */
struct NoDirectory : virtual public File
{
	NoDirectory()
	{}
	NoDirectory( const std::string path )
	{
		*this << filename(path);
	}
};

/**
 * @brief Read only file.
 */
struct ReadOnlyFile : virtual public File
{
	ReadOnlyFile()
	{}
	ReadOnlyFile( const std::string path )
	{
		*this << filename(path);
	}
};
/// @}

}
}

#endif
