#ifndef _TUTTLE_HOST_EXCEPTION_HPP_
#define _TUTTLE_HOST_EXCEPTION_HPP_

#include <tuttle/common/exceptions.hpp>

#define TUTTLE_COUT_EXCEPTION( e )  \
    CERR( "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::diagnostic_information( e ) )

#define TUTTLE_COUT_CURRENT_EXCEPTION  \
    CERR( "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::current_exception_diagnostic_information() )

namespace tuttle {
namespace host {
namespace exception {

using namespace tuttle::exception;

}
}
}

#endif
