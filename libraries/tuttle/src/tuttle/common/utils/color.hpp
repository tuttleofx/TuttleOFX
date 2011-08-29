#ifndef _TUTTLE_COMMON_COLOR_HPP_
#define	_TUTTLE_COMMON_COLOR_HPP_

#include <string>

namespace tuttle {
namespace common {

#ifdef __LINUX__
static const std::string kColorStd      ( "\E[0;0m"  );
static const std::string kColorFolder   ( "\E[1;34m" );
static const std::string kColorFile     ( "\E[0;32m" );
static const std::string kColorSequence ( "\E[0;32m" );
static const std::string kColorError    ( "\E[1;31m" );
#else
static const std::string kColorStd      ( "" );
static const std::string kColorFolder   ( "" );
static const std::string kColorFile     ( "" );
static const std::string kColorSequence ( "" );
static const std::string kColorError    ( "" );
#endif

}
}

#endif

