#ifndef _SAM_COLOR_HPP_
#define	 _SAM_COLOR_HPP_

#include <tuttle/common/utils/color.hpp>

#include <string>
#include <iostream>
namespace sam {

struct Color
{
	std::string _blue;
	std::string _green;
	std::string _red;
	
	std::string _folder;
	std::string _file;

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
		
		_folder = kColorFolder;
		_file   = kColorFile;

		_std    = kColorStd;
		_error  = kColorError;
	}
	
};

}

#endif

