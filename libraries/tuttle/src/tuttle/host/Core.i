%include <tuttle/host/global.i>
%include <tuttle/host/Preferences.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/memory/MemoryCache.i>
%include <tuttle/host/memory/MemoryPool.i>
%include <tuttle/host/ofx/OfxhPlugin.i>
%include <tuttle/host/ofx/OfxhPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/common/patterns/Singleton.i>

%include <std_vector.i>
%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/host/Core.hpp>
%}

%template(SingletonCore) Singleton<tuttle::host::Core>;

%include <tuttle/host/Core.hpp>

%extend tuttle::host::Core
{
	const ofx::OfxhPlugin& __getitem__( const std::string& name ) const
	{
		return *self->getPluginCache().getPluginById( name );
	}

	std::string __str__() const
	{
		std::stringstream s;
		s << *self;
		return s.str();
	}
}
