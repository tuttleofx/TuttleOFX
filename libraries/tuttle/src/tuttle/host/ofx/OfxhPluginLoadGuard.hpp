#ifndef OFXH_PLUGINHANDLE_HPP
#define OFXH_PLUGINHANDLE_HPP

#include "OfxhHost.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <ofxCore.h>

namespace tuttle
{
namespace host
{
namespace ofx
{

/**
 * Wrapper class for Plugin/PluginBinary. Use in a RAII fashion to make sure the binary gets unloaded when needed.
 */
class OfxhPluginLoadGuard
{
    OfxhPlugin* _plugin;
    OfxhPluginBinary* _pluginBinary;

    OfxPlugin* _rawOfxPlugin;

public:
    OfxhPluginLoadGuard(OfxhPlugin& p, OfxhHost& _host);
    virtual ~OfxhPluginLoadGuard();

    OfxPlugin* getOfxPlugin() { return _rawOfxPlugin; }
    const OfxPlugin* getOfxPlugin() const { return _rawOfxPlugin; }

    OfxPlugin* operator->() { return _rawOfxPlugin; }
};
}
}
}

#endif
