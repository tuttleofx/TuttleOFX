%include <tuttle/host/global.i>

%{
#include <tuttle/host/Preferences.hpp>
%}

%include <tuttle/host/Preferences.hpp>

%extend tuttle::host::Preferences
{
	std::string buildTuttleTestPathStr()
	{
		return self->buildTuttleTestPath().string();
	}
}


