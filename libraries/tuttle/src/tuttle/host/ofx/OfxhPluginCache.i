%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhPluginAPICache.i>
%include <tuttle/host/ofx/OfxhPlugin.i>
%include <tuttle/host/ofx/OfxhPluginBinary.i>

%include <std_list.i>
%include <std_set.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
%}

%extend tuttle::host::ofx::OfxhPluginCache
{
	OfxhPlugin& __getitem__( const std::string& name )
	{
		return *self->getPluginById( name );
	}

	std::string __str__() const
	{
		std::stringstream s;

		s << *self;
		return s.str();
	}
}

namespace std {
%template(OfxhPluginPtrList) list<tuttle::host::ofx::OfxhPlugin*>;
}

%include <tuttle/host/ofx/OfxhPluginCache.hpp>

