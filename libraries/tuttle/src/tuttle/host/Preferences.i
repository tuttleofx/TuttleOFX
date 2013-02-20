%include <tuttle/host/global.i>

%include <std_string.i>

%{
#include <tuttle/host/Preferences.hpp>
%}

//%rename(private_buildTuttleTestPath) buildTuttleTestPath;

%include <tuttle/host/Preferences.hpp>


%extend tuttle::host::Preferences
{
	std::string buildTuttleTestPathStr()
	{
		return self->buildTuttleTestPath().string();
	}
}


