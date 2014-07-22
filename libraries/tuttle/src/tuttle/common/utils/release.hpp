#ifndef _TUTTLE_COMMON_RELEASE_HPP_
#define _TUTTLE_COMMON_RELEASE_HPP_

/*
 * In release mode, TUTTLE_LOG_*_DEBUG are disabled.
 */

/// @see TUTTLE_LOG
#define TUTTLE_LOG_DEBUG(... )

#define TUTTLE_LOG_TRACE_DEBUG(... )
#define TUTTLE_LOG_INFO_DEBUG(... )

/// @see  TUTTLE_LOG_INFOS
#define TUTTLE_LOG_INFOS_DEBUG
/// @see  TUTTLE_LOG_INFOS
#define TUTTLE_LOG_WITHINFOS_DEBUG(... )
/// @see TUTTLE_IF_DEBUG
#define TUTTLE_IF_DEBUG(... )

#define TUTTLE_LOG_VAR_DEBUG(... )
#define TUTTLE_LOG_VAR2_DEBUG(... )
#define TUTTLE_LOG_VAR3_DEBUG(... )
#define TUTTLE_LOG_VAR4_DEBUG(... )
#define TUTTLE_LOG_WITHINFOS_DEBUG(... )
#define TUTTLE_LOG_WARNING_DEBUG(... )
#define TUTTLE_LOG_ERROR_DEBUG(... )
#define TUTTLE_LOG_FATALERROR_DEBUG(... )
#define TUTTLE_LOG_EXCEPTION_DEBUG(... )

#endif
