#ifndef _TUTTLE_COMMON_DEBUG_HPP_
#define _TUTTLE_COMMON_DEBUG_HPP_

// pre-processeur directives :
//	# : convert to string
//	## : concatenate (or before __VA_ARGS__ to indicate that it may be empty)
//	__FILE__ : filename
//	__LINE__ : line number
//	__FUNCTION__ : function declaration
//	__PRETTY_FUNCTION__ : function name
//	__DATE__ : "Mmm dd yyyy"
//	__TIME__ : "hh:mm:ss"

//_____________________________________________________________________________
// Macros to output on terminal only in debug mode

/// @see TUTTLE_LOG
#define TUTTLE_LOG_DEBUG TUTTLE_LOG

#define TUTTLE_LOG_TRACE_DEBUG TUTTLE_LOG_TRACE
#define TUTTLE_LOG_INFO_DEBUG TUTTLE_LOG_INFO

/// @see  TUTTLE_LOG_INFOS
#define TUTTLE_LOG_INFOS_DEBUG TUTTLE_LOG_INFOS

/// @see  TUTTLE_LOG_INFOS
#define TUTTLE_LOG_WITHINFOS_DEBUG TUTTLE_LOG_WITHINFOS

/// @see TUTTLE_IF_DEBUG
#define TUTTLE_IF_DEBUG(... )  __VA_ARGS__

#define TUTTLE_LOG_VAR_DEBUG TUTTLE_LOG_VAR
#define TUTTLE_LOG_VAR2_DEBUG TUTTLE_LOG_VAR2
#define TUTTLE_LOG_VAR3_DEBUG TUTTLE_LOG_VAR3
#define TUTTLE_LOG_VAR4_DEBUG TUTTLE_LOG_VAR4
#define TUTTLE_LOG_INFOS_DEBUG TUTTLE_LOG_INFOS
#define TUTTLE_LOG_WITHINFOS_DEBUG TUTTLE_LOG_WITHINFOS
#define TUTTLE_LOG_WARNING_DEBUG TUTTLE_LOG_WARNING
#define TUTTLE_LOG_ERROR_DEBUG TUTTLE_LOG_ERROR
#define TUTTLE_LOG_FATALERROR_DEBUG TUTTLE_LOG_FATALERROR
#define TUTTLE_LOG_EXCEPTION_DEBUG TUTTLE_LOG_EXCEPTION

#ifdef __WINDOWS__
 #include <tuttle/common/system/windows/MemoryLeaks.hpp>
#endif

#endif

