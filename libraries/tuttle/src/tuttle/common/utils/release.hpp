#ifndef __TUTTLE_RELEASE__
#define __TUTTLE_RELEASE__

/*
 * In release mode, TUTTLE_COUT_*_DEBUG are disabled.
 */

/// @see TUTTLE_COUT
#define TUTTLE_COUT_DEBUG(... )
/// @see  TUTTLE_COUT_INFOS
#define TUTTLE_COUT_INFOS_DEBUG
/// @see  TUTTLE_COUT_INFOS
#define TUTTLE_COUT_WITHINFOS_DEBUG(... )
/// @see IF_DEBUG
#define IF_DEBUG(... )

#define TUTTLE_COUT_X_DEBUG( N, ... )
#define TUTTLE_COUT_VAR_DEBUG(... )
#define TUTTLE_COUT_VAR2_DEBUG(... )
#define TUTTLE_COUT_VAR3_DEBUG(... )
#define TUTTLE_COUT_VAR4_DEBUG(... )
#define TUTTLE_COUT_WITHINFOS_DEBUG(... )
#define TUTTLE_COUT_WARNING_DEBUG(... )
#define TUTTLE_COUT_ERROR_DEBUG(... )
#define TUTTLE_COUT_FATALERROR_DEBUG(... )
#define TUTTLE_COUT_EXCEPTION_DEBUG(... )

#endif
