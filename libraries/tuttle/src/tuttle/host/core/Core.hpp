#ifndef _TUTTLE_CORE_HPP_
#define _TUTTLE_CORE_HPP_

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

#include <tuttle/common/patterns/Singleton.hpp>

namespace tuttle {
namespace host {
namespace core {

class Core : public Singleton<Core>
{
public:
	friend class Singleton<Core>;
private:
	Core( ) : _imageEffectPluginCache(_host) { }
	~Core( ) { }

private:
	tuttle::host::core::Host _host;
	tuttle::host::ofx::imageEffect::ImageEffectPluginCache _imageEffectPluginCache;

public:
	tuttle::host::core::Host& getHost() { return _host; }
	
public:
	void preload();

};

}
}
}

#endif
