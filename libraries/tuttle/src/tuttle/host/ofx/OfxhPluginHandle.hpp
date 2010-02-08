#ifndef OFXH_PLUGINHANDLE_HPP
#define OFXH_PLUGINHANDLE_HPP

#include "OfxhHost.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <ofxCore.h>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * wrapper class for Plugin/PluginBinary.  use in a RAIA fashion to make sure the binary gets unloaded when needed and not before.
 */
class OfxhPluginHandle
{
OfxhPlugin* _p;
OfxhPluginBinary* _b;
OfxPlugin* _op;

public:
	OfxhPluginHandle( OfxhPlugin* p, OfxhHost* _host );
	virtual ~OfxhPluginHandle();

	OfxPlugin*       getOfxPlugin()       { return _op; }
	const OfxPlugin* getOfxPlugin() const { return _op; }

	OfxPlugin* operator->() { return _op; }

};

}
}
}

#endif

