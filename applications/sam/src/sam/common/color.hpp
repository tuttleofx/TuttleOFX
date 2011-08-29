#ifndef _SAM_COLOR_HPP_
#define	_SAM_COLOR_HPP_

#include <tuttle/common/utils/color.hpp>

#include <string>

namespace sam {

struct Color
{
	std::string _blue;
	std::string _green;
	std::string _red;
	
	std::string _std;
	std::string _error;
	
	void disable()
	{
		*this = Color();
	}
	void enable()
	{
		using namespace tuttle::common;
		_blue   = kColorFolder;
		_green  = kColorFile;
		_red    = kColorError;
		
		_std    = kColorStd;
		_error  = kColorError;
	}
	
};

}

#endif

