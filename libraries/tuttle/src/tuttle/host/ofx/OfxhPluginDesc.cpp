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
_identifier( identifier ),
_rawIdentifier( rawIdentifier ),
_versionMajor( versionMajor ),
_versionMinor( versionMinor ) { }

/**
 * constructor for the case where we have already loaded the plugin binary and
 * are populating this object from it
 */
OfxhPluginDesc::OfxhPluginDesc( OfxPlugin* ofxPlugin )
: _pluginApi( ofxPlugin->pluginApi ),
_apiVersion( ofxPlugin->apiVersion ),
_identifier( ofxPlugin->pluginIdentifier ),
_rawIdentifier( ofxPlugin->pluginIdentifier ),
_versionMajor( ofxPlugin->pluginVersionMajor ),
_versionMinor( ofxPlugin->pluginVersionMinor )
{
	boost::to_lower( _identifier );
}

bool OfxhPluginDesc::operator==( const This& other ) const
{
	if( _pluginApi != other._pluginApi ||
		_apiVersion != other._apiVersion ||
		_identifier != other._identifier ||
		_rawIdentifier != other._rawIdentifier ||
		_versionMajor != other._versionMajor ||
		_versionMinor != other._versionMinor )
		return false;
	return true;
}

}
}
}

