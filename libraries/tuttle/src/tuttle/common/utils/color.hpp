#ifndef _TUTTLE_COMMON_COLOR_HPP_
#define	_TUTTLE_COMMON_COLOR_HPP_

#include <string>

namespace tuttle {
namespace common {

#if (defined __UNIX__ || defined UNIX || defined __LINUX__ || defined LINUX )
static const std::string kColorBlack    ( "\E[1;30m" );
static const std::string kColorWhite    ( "\E[1;37m" );
static const std::string kColorBlue     ( "\E[1;34m" );
static const std::string kColorGreen    ( "\E[32m" );
static const std::string kColorRed      ( "\E[1;31m" );
static const std::string kColorCyan     ( "\E[1;36m" );
static const std::string kColorMagenta  ( "\E[1;35m" );
static const std::string kColorYellow   ( "\E[33m" );//yellow isn't visible on light background, that's why a darker color is used here

static const std::string kColorStd      ( "\E[0;0m"   );
static const std::string kColorFolder   ( kColorBlue  );
static const std::string kColorFile     ( kColorGreen );
static const std::string kColorSequence ( kColorGreen );
static const std::string kColorError    ( kColorRed   );
#else
static const std::string kColorBlack    ( "" );
static const std::string kColorWhite    ( "" );
static const std::string kColorBlue     ( "" );
static const std::string kColorGreen    ( "" );
static const std::string kColorRed      ( "" );
static const std::string kColorCyan     ( "" );
static const std::string kColorMagenta  ( "" );
static const std::string kColorYellow   ( "" );

static const std::string kColorStd      ( "" );
static const std::string kColorFolder   ( "" );
static const std::string kColorFile     ( "" );
static const std::string kColorSequence ( "" );
static const std::string kColorError    ( "" );
#endif

}
}

#endif

