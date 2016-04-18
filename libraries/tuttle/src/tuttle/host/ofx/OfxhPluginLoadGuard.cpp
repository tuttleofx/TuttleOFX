#include "OfxhPluginLoadGuard.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{

typedef OfxPlugin* (*OfxGetPluginType)(int);

OfxhPluginLoadGuard::OfxhPluginLoadGuard(OfxhPlugin& plugin, OfxhHost& host)
    : _plugin(&plugin)
    , _pluginBinary(&plugin.getBinary())
    , _rawOfxPlugin(0)
{
    _pluginBinary->_binary.ref();
    OfxGetPluginType getPlugin_func = (OfxGetPluginType)_pluginBinary->_binary.findSymbol("OfxGetPlugin");
    _rawOfxPlugin = getPlugin_func(_plugin->getIndex());

    if(!_rawOfxPlugin)
    {
        // We throw inside the constructor, so the destructor will not be called.
        _pluginBinary->_binary.unref();

        BOOST_THROW_EXCEPTION(exception::Data()
                              << exception::user("Loading plugin failed.")
                              << exception::dev() + "OfxGetPlugin call failed at index " + _plugin->getIndex() + "."
                              << exception::filename(_pluginBinary->getBundlePath()));
    }
    _rawOfxPlugin->setHost(host.getHandle());
}

OfxhPluginLoadGuard::~OfxhPluginLoadGuard()
{
    _pluginBinary->_binary.unref();
}
}
}
}
