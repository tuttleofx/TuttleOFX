%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhImageEffectHost.i>
%include <tuttle/host/ofx/OfxhPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
%include <tuttle/host/ofx/OfxhMajorPlugin.i>

%include <std_vector.i>
%include <std_list.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhMajorPlugin.hpp>
%}

namespace std {
%template(OfxhImageEffectPluginPtrVector) vector<tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
%template(OfxhImageEffectPluginByStrMap) map<std::string, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
%template(OfxhImageEffectPluginByMajorPluginMap) map<tuttle::host::ofx::OfxhMajorPlugin, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
}

%include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>


%extend tuttle::host::ofx::OfxhImageEffectPluginCache
{
	OfxhImageEffectPlugin& __getitem__( const std::string& name )
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


