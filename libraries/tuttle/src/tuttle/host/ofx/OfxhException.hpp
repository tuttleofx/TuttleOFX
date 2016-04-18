#ifndef _OFXH_EXCEPTION_HPP_
#define _OFXH_EXCEPTION_HPP_

#include "OfxhCore.hpp"
#include "OfxhUtilities.hpp"

#include <tuttle/host/exceptions.hpp>

#include <boost/throw_exception.hpp>
#include <boost/exception/exception.hpp>

#include <stdexcept>

namespace tuttle
{
namespace host
{
namespace ofx
{

/**
 * exception, representing an OfxStatus
 */
class OfxhException : virtual public ::std::exception, virtual public ::boost::exception, virtual public ::boost::backtrace
{
    OfxStatus _stat;

public:
    explicit OfxhException(const std::string& what)
        : ::boost::exception()
        , _stat(kOfxStatErrUnknown)
    {
        *this << ::tuttle::exception::dev() + what;
    }

    explicit OfxhException(OfxStatus stat)
        : ::boost::exception()
        , _stat(stat)
    {
        *this << ::tuttle::exception::ofxStatus(stat);
    }

    explicit OfxhException(OfxStatus stat, const std::string& what)
        : ::boost::exception()
        , _stat(stat)
    {
        *this << ::tuttle::exception::ofxStatus(stat);
        *this << ::tuttle::exception::dev() + what;
    }

    /// get the status
    OfxStatus getStatus() const { return _stat; }

    ///@todo tuttle : mapStatusEnumToStr... in tuttle common
    std::string getStatusStr() const { return ofx::mapStatusToString(_stat); }
};

//#if !defined(WINDOWS) || !defined(SWIG)
// #define OFX_EXCEPTION_SPEC throw(tuttle::host::ofx::OfxhException)
//#else
#define OFX_EXCEPTION_SPEC
//#endif
}
}
}

#endif
