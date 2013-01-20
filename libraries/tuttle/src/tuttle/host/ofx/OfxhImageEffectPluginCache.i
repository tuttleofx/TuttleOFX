%include <tuttle/host/global.i>
%include <tuttle/host/ofx/OfxhImageEffectHost.i>
%include <tuttle/host/ofx/OfxhPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
%include <tuttle/host/ofx/OfxhMajorPlugin.i>

%include <std_vector.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>
%}


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

namespace std {
typedef OfxhImageEffectPluginPtrVector vector<tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
typedef OfxhImageEffectPluginByStrMap map<std::string, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
typedef OfxhImageEffectPluginByMajorPluginMap map<OfxhMajorPlugin, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*>;
}

%include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>



