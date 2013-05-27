#ifndef _TUTTLE_COMMON_COLOR_HPP_
#define	_TUTTLE_COMMON_COLOR_HPP_

#include <tuttle/common/system/system.hpp>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace tuttle {
namespace common {

namespace details {
#if defined __LINUX__ || defined __MACOS__
static const std::string kColorBlack    ( "\E[1;30m" );
static const std::string kColorWhite    ( "\E[1;37m" );
static const std::string kColorBlue     ( "\E[1;34m" );
static const std::string kColorGreen    ( "\E[0;32m" );
static const std::string kColorRed      ( "\E[1;31m" );
static const std::string kColorCyan     ( "\E[1;36m" );
static const std::string kColorMagenta  ( "\E[1;35m" );
static const std::string kColorYellow   ( "\E[0;33m" );//yellow isn't visible on light background, that's why a darker color is used here

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

class Color : boost::noncopyable
{
private:
	Color( ) { }
	
public:
	
	static boost::shared_ptr<Color> get();
	
	~Color(){}
	
	std::string _blue;
	std::string _green;
	std::string _yellow;
	std::string _red;

	std::string _folder;
	std::string _file;

	std::string _std;
	std::string _error;

	void disable()
	{
		_blue.clear();
		_green.clear();
		_red.clear();
		_yellow.clear();

		_folder.clear();
		_file.clear();

		_std.clear();
		_error.clear();
	}
	void enable()
	{
		using namespace common::details;
		_blue   = kColorBlue;
		_green  = kColorGreen;
		_red    = kColorRed;
		_yellow = kColorYellow;

		_folder = kColorFolder;
		_file   = kColorFile;

		_std    = kColorStd;
		_error  = kColorError;
	}

public:
	static boost::shared_ptr<Color> color;
};

}
}

#endif

