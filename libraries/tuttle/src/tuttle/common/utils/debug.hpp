#ifndef _TUTTLE_COMMON_DEBUG_HPP_
#define _TUTTLE_COMMON_DEBUG_HPP_

#include <tuttle/common/system/system.hpp>

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

/// @see TUTTLE_COUT
#define TUTTLE_COUT_DEBUG TUTTLE_COUT

/// @see  TUTTLE_COUT_INFOS
#define TUTTLE_COUT_INFOS_DEBUG TUTTLE_COUT_INFOS

/// @see  TUTTLE_COUT_INFOS
#define TUTTLE_COUT_WITHINFOS_DEBUG TUTTLE_COUT_WITHINFOS

/// @see TUTTLE_IF_DEBUG
#define TUTTLE_IF_DEBUG(... )  __VA_ARGS__

#define TUTTLE_COUT_X_DEBUG TUTTLE_COUT_X
#define TUTTLE_COUT_VAR_DEBUG TUTTLE_COUT_VAR
#define TUTTLE_COUT_VAR2_DEBUG TUTTLE_COUT_VAR2
#define TUTTLE_COUT_VAR3_DEBUG TUTTLE_COUT_VAR3
#define TUTTLE_COUT_VAR4_DEBUG TUTTLE_COUT_VAR4
#define TUTTLE_COUT_INFOS_DEBUG TUTTLE_COUT_INFOS
#define TUTTLE_COUT_WITHINFOS_DEBUG TUTTLE_COUT_WITHINFOS
#define TUTTLE_COUT_WARNING_DEBUG TUTTLE_COUT_WARNING
#define TUTTLE_COUT_ERROR_DEBUG TUTTLE_COUT_ERROR
#define TUTTLE_COUT_FATALERROR_DEBUG TUTTLE_COUT_FATALERROR
#define TUTTLE_COUT_EXCEPTION_DEBUG TUTTLE_COUT_EXCEPTION

#ifdef __WINDOWS__
 #include "windows/MemoryLeaks.hpp"
#endif

#endif

