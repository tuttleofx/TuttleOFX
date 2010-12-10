#ifndef __TUTTLE_RELEASE__
#define __TUTTLE_RELEASE__

/*
 * In release mode, COUT_*_DEBUG are disabled.
 */

/// @see COUT
#define COUT_DEBUG(... )
/// @see  COUT_INFOS
#define COUT_INFOS_DEBUG
/// @see  COUT_INFOS
#define COUT_WITHINFOS_DEBUG(... )
/// @see IF_DEBUG
#define IF_DEBUG(... )

#define COUT_X_DEBUG( N, ... )
#define COUT_VAR_DEBUG(... )
#define COUT_VAR2_DEBUG(... )
#define COUT_VAR3_DEBUG(... )
#define COUT_VAR4_DEBUG(... )
#define COUT_WITHINFOS_DEBUG(... )
#define COUT_WARNING_DEBUG(... )
#define COUT_ERROR_DEBUG(... )
#define COUT_FATALERROR_DEBUG(... )
#define COUT_EXCEPTION_DEBUG(... )

#endif
