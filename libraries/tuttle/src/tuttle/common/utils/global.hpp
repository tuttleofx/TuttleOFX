#ifndef _TUTTLE_COMMON_UTILS_GLOBAL_HPP_
#define _TUTTLE_COMMON_UTILS_GLOBAL_HPP_

#include "color.hpp"
#include "Formatter.hpp"

////////////////////////////////////////////////////////////////////////////////
// System stuff
#include <tuttle/common/system/system.hpp>
#include <tuttle/common/system/compatibility.hpp>
#include <tuttle/common/system/windows/windows.h>

////////////////////////////////////////////////////////////////////////////////
// Assert needs to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console
#include <iostream>

#ifdef NDEBUG
#if defined(_MSC_VER)
#define TUTTLE_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#define TUTTLE_FORCEINLINE inline __attribute__((always_inline))
#else
#define TUTTLE_FORCEINLINE inline
#endif
#else
#define TUTTLE_FORCEINLINE inline
#endif

#define TUTTLE_COUT(...) std::cout << __VA_ARGS__ << std::endl

#ifndef WITHOUT_BOOST_LOG
#define TUTTLE_LOG(MODE, ...) MODE << __VA_ARGS__

#define TUTTLE_TRACE BOOST_LOG_TRIVIAL(trace)
#define TUTTLE_DEBUG BOOST_LOG_TRIVIAL(debug)
#define TUTTLE_INFO BOOST_LOG_TRIVIAL(info)
#define TUTTLE_WARNING BOOST_LOG_TRIVIAL(warning)
#define TUTTLE_ERROR BOOST_LOG_TRIVIAL(error)
#define TUTTLE_FATAL BOOST_LOG_TRIVIAL(fatal)
#else
#define TUTTLE_LOG(MODE, ...) TUTTLE_COUT(MODE << __VA_ARGS__)

#define TUTTLE_TRACE "Trace: "
#define TUTTLE_DEBUG "Debug: "
#define TUTTLE_INFO "Info: "
#define TUTTLE_WARNING "Warning: "
#define TUTTLE_ERROR "Error: "
#define TUTTLE_FATAL "Fatal: "
#endif

/**
 * @def   TUTTLE_INFOS
 * @brief informations : filename, line number, function name
 **/

#define TUTTLE_GET_INFOS_FILE "in file:  " << __FILE__ << ",  line: " << __LINE__
#define TUTTLE_GET_INFOS_FUNCTION "function: " << BOOST_CURRENT_FUNCTION
#define TUTTLE_GET_INFOS TUTTLE_GET_INFOS_FILE << TUTTLE_GET_INFOS_FUNCTION

#define TUTTLE_GET_VAR(a) #a << ": " << a
#define TUTTLE_GET_VAR2(a, b) TUTTLE_GET_VAR(a) << ", " << TUTTLE_GET_VAR(b)
#define TUTTLE_GET_VAR3(a, b, c) TUTTLE_GET_VAR(a) << ", " << TUTTLE_GET_VAR(b) << ", " << TUTTLE_GET_VAR(c)
#define TUTTLE_GET_VAR4(a, b, c, d)                                                                                         \
    TUTTLE_GET_VAR(a) << ", " << TUTTLE_GET_VAR(b) << ", " << TUTTLE_GET_VAR(c) << ", " << TUTTLE_GET_VAR(d)

/**
 * @param[in] ... : all parameters with an output stream operator defined
 * @brief terminal display
 **/

#define TUTTLE_LOG_TRACE(...) TUTTLE_LOG(TUTTLE_TRACE, __VA_ARGS__)
#define TUTTLE_LOG_DEBUG(...) TUTTLE_LOG(TUTTLE_DEBUG, __VA_ARGS__)
#define TUTTLE_LOG_INFO(...) TUTTLE_LOG(TUTTLE_INFO, __VA_ARGS__)
#define TUTTLE_LOG_WARNING(...)                                                                                             \
    TUTTLE_LOG(TUTTLE_WARNING, tuttle::common::Color::get()->_yellow << __VA_ARGS__ << tuttle::common::Color::get()->_std)
#define TUTTLE_LOG_ERROR(...)                                                                                               \
    TUTTLE_LOG(TUTTLE_ERROR, tuttle::common::Color::get()->_error << __VA_ARGS__ << tuttle::common::Color::get()->_std)
#define TUTTLE_LOG_FATAL(...)                                                                                               \
    TUTTLE_LOG(TUTTLE_FATAL, tuttle::common::Color::get()->_error << __VA_ARGS__ << tuttle::common::Color::get()->_std)

#define TUTTLE_LOG_VAR(MODE, a) TUTTLE_LOG(MODE, TUTTLE_GET_VAR(a))
#define TUTTLE_LOG_VAR2(MODE, a, b) TUTTLE_LOG(MODE, TUTTLE_GET_VAR2(a, b))
#define TUTTLE_LOG_VAR3(MODE, a, b, c) TUTTLE_LOG(MODE, TUTTLE_GET_VAR3(a, b, c))
#define TUTTLE_LOG_VAR4(MODE, a, b, c, d) TUTTLE_LOG(MODE, TUTTLE_GET_VAR4(a, b, c, d))

/**
 * @brief terminal information display
 **/
#define TUTTLE_LOG_INFOS                                                                                                    \
    TUTTLE_LOG_TRACE(TUTTLE_GET_INFOS_FILE);                                                                                \
    TUTTLE_LOG_TRACE(TUTTLE_GET_INFOS_FUNCTION)

////////////////////////////////////////////////////////////////////////////////
// Some specifics things to debug or release version
#ifdef DEBUG

// Create "*.dot" file during the process to get informations about the graph
#define TUTTLE_EXPORT_PROCESSGRAPH_DOT 1

// Log several times during the process
#define TUTTLE_EXPORT_WITH_TIMER 1

// Output all nodes in the graph as png
#define TUTTLE_PNG_EXPORT_BETWEEN_NODES 1

// Init dst buffer with red to highlight uninitialized pixels
#define TUTTLE_INIT_IMAGE_BUFFERS 1

#else

#define TUTTLE_EXPORT_PROCESSGRAPH_DOT 0
#define TUTTLE_EXPORT_WITH_TIMER 0
#define TUTTLE_PNG_EXPORT_BETWEEN_NODES 0
#define TUTTLE_INIT_IMAGE_BUFFERS 0

#endif

#define TUTTLE_LOG_PLUGIN_NAME_WIDTH 30

#endif
