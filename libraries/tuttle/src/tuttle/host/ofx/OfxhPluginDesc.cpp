#include "OfxhPluginDesc.hpp"

namespace tuttle {
namespace host {
namespace ofx {

OfxhPluginDesc::OfxhPluginDesc( ) { }

OfxhPluginDesc::~OfxhPluginDesc( ) { }

OfxhPluginDesc::OfxhPluginDesc( const std::string& api,
				int apiVersion,
				const std::string& identifier,
				const std::string& rawIdentifier,
				int versionMajor,
				int versionMinor )
: _pluginApi( api ),
_apiVersion( apiVersion ),
_ident( identifier, rawIdentifier, versionMinor, versionMajor )
{ }

/**
 * constructor for the case where we have already loaded the plugin binary and
 * are populating this object from it
 */
OfxhPluginDesc::OfxhPluginDesc( OfxPlugin& ofxPlugin )
: _pluginApi( ofxPlugin.pluginApi ),
_apiVersion( ofxPlugin.apiVersion ),
_ident( ofxPlugin.pluginIdentifier, ofxPlugin.pluginIdentifier, ofxPlugin.pluginVersionMinor, ofxPlugin.pluginVersionMajor )
{
	boost::to_lower( _ident._identifier );
}

bool OfxhPluginDesc::operator==( const This& other ) const
{
	if( _pluginApi != other._pluginApi ||
		_apiVersion != other._apiVersion ||
		_ident != other._ident )
		return false;
	return true;
}

}
}
}

