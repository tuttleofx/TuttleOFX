#include "OfxhPluginBinary.hpp"
#include "OfxhPluginCache.hpp"

#include <tuttle/host/exceptionsHost.hpp>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * @brief try to open the plugin bundle object and query it for plugins
 */
void OfxhPluginBinary::loadPluginInfo( OfxhPluginCache* cache )
{
	_fileModificationTime = _binary.getTime();
	_fileSize             = _binary.getSize();
	_binaryChanged        = false;

	_binary.load();

	int ( *getNo )( void )       = ( int( * ) () )_binary.findSymbol( "OfxGetNumberOfPlugins" );
	OfxPlugin* ( *getPlug )(int) = ( OfxPlugin * ( * )( int ) )_binary.findSymbol( "OfxGetPlugin" );

	if( getNo == 0 || getPlug == 0 )
	{
		_binary.setInvalid( true );
	}
	else
	{
		int pluginCount = ( *getNo )( );

		_plugins.reserve( pluginCount );

		for( int i = 0; i < pluginCount; ++i )
		{
			OfxPlugin& plug = *( *getPlug )( i );

			APICache::OfxhPluginAPICacheI* api = cache->findApiHandler( plug.pluginApi, plug.apiVersion );
			assert( api );

			OfxhPlugin* newPlug = api->newPlugin( *this, i, plug );
			if( newPlug == NULL )
			{
				BOOST_THROW_EXCEPTION( exception::Unknown()
				    << exception::dev( "Error creating a new OfxhPlugin." )
				    << exception::pluginIdentifier( plug.pluginIdentifier )
				    << exception::ofxApi( plug.pluginApi )
				    << exception::filename( _binary.getBinaryPath() ) );
			}
			_plugins.push_back( newPlug );
		}
	}
	_binary.unload();
}

OfxhPluginBinary::~OfxhPluginBinary()
{}

}
}
}

