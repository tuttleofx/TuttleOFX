#ifndef _TUTTLE_HOST_EXCEPTION_HPP_
#define _TUTTLE_HOST_EXCEPTION_HPP_

#include <tuttle/host/ofx/OfxhUtilities.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_file_name.hpp>

#include <ofxCorePlugin.h>
#include <ofxImageEffect.h>

#include <sstream>
#include <string>
#include <stdexcept>


#define COUT_EXCEPTION( e )  \
    ::std::cerr << "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::diagnostic_information(e) << ::std::endl

#define COUT_CURRENT_EXCEPTION  \
    ::std::cerr << "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::current_exception_diagnostic_information() << ::std::endl

#ifndef SWIG
namespace boost {

	struct error_info_sstream
	{
        typedef std::ostringstream value_type;
        value_type _v;
	};

    inline
    std::string
    to_string( const error_info_sstream& x )
	{
		return x._v.str();
	}

    template<class Tag>
    class error_info<Tag, error_info_sstream>: public exception_detail::error_info_base
	{
	public:
        typedef error_info_sstream T;
        typedef error_info<Tag,T> This;
        typedef T value_type;

		error_info(){}
		error_info( const This& v )
		{
			_value._v << v._value._v.str();
		}
		template<typename V>
        error_info( const V& value )
		{
			_value._v << value;
		}
        ~error_info() throw() {}

		template<typename V>
		This& operator+( const V& v )
		{
			_value._v << v;
			return *this;
		}
		
        const value_type& value() const { return _value; }
        value_type& value() { return _value; }
	private:
        std::string tag_typeid_name() const { return tag_type_name<Tag>(); }
        std::string value_as_string() const { return _value._v.str(); }

        value_type _value;
	};
}
#endif

namespace tuttle {
namespace host {

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
 * @remark We keep lower camel case for tags,
 *	to keep a diffrence between tags and exception types
 */
/// @{
/**
 * @brief If you catch an error at the top level, you can print this information to the user.
 * @remark User information.
 */
typedef ::boost::error_info<struct tag_userMessage,::boost::error_info_sstream> user;
/**
 * @brief This is detailed informations for developpers.
 * Not always a real human readable message :)
 * @remark Dev information.
 */
//typedef ::boost::error_info<struct tag_message,std::string> dev;
typedef ::boost::error_info<struct tag_devMessage,::boost::error_info_sstream> dev;
//typedef ::boost::error_info_sstream<struct tag_message> dev;
/**
 * @brief The ofx error status code.
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_ofxStatus,::OfxStatus> ofxStatus;
/**
 * @brief The ofx context name.
 * Each plugin can be instanciated in differents contexts (depending on the declaration of supported contexts).
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_ofxContext,::std::string> ofxContext;
/**
 * @brief The ofx api string identification.
 * @remark Dev information.
 */
typedef ::boost::error_info<struct tag_ofxApi,::std::string> ofxApi;
/**
 * @brief Plugin string identifier.
 * @remark Dev or User information.
 */
typedef ::boost::error_info<struct tag_pluginIdentifier,::std::string> pluginIdentifier;
/**
 * @brief Plugin name.
 * @remark User information.
 */
typedef ::boost::error_info<struct tag_pluginName,::std::string> pluginName;
/**
 * @brief Problem with a file.
 * @remark User information.
 */
typedef ::boost::errinfo_file_name filename;
/// @}
#endif


/** @brief Common exception for all tuttle plugin exceptions */
struct Common : virtual public ::std::exception, virtual public ::boost::exception {};

/// @brief Ofx standard errors
/// @{

/**
 * @brief You have to specify the exception::ofxStatus(kOfxStatXXX) yourself.
 * When you call a base level function (C API) which returns an ofxStatus, you can use this exception and fill it with the returned value using ofxStatus tag.
 */
struct OfxCustom : virtual public Common {};

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


/// @brief Other host exceptions
/// @{

/**
 * @brief The class serves as the base class for all exceptions thrown to report errors presumably detectable before the program executes, such as violations of logical preconditions (cf. std::logic_error).
 * @remark With this exception, you normally have a "user" tag message.
 */
struct Logic : virtual public Value {};

/**
 * @brief Something that should never appends.
 * These exceptions may be replaced by assertions, but we prefer to keep a runtime check even in release (for the moment).
 * @remark With this exception, you normally have a "dev" tag message.
 */
struct Bug : virtual public Value {};

/** @brief Unknown error inside a conversion. */
struct BadConversion : virtual public Value {};

/** @brief Error with memory cache or memory pool. */
struct ImageNotReady : virtual public Value {};

/**
 * @brief A non optional input clip in not connected.
 * We can't launch the render in this case.
 */
struct ImageNotConnected : virtual public Value {};

/**
 * @brief Input property don't satisfy descriptor requirements.
 * We can't launch the render in this case.
 */
struct InputMismatch : virtual public Format {};

/**
 * @brief Input and output properties mismatch.
 * We can't launch the render in this case.
 */
struct InOutMismatch : virtual public Format {};

/**
 * @brief Input(s) and enventually output properties mismatch.
 * We can't launch the render in this case.
 */
struct BitDepthMismatch : virtual public ImageFormat {};

/**
 * @brief Image raw bytes not valid.
 * We can't launch the render in this case.
 */
struct WrongRowBytes : virtual public ImageFormat {};

/** @brief Status error code indicating that something failed due an illegal data */
struct Data : virtual public Value {};

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
