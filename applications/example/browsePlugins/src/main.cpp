#include <tuttle/host/Graph.hpp>
#include <boost/foreach.hpp>
#include <iostream>

int main( int argc, char** argv )
{
	using namespace tuttle::host;
	core().preload();

	BOOST_FOREACH( ofx::OfxhPlugin* plugin, core().getPluginCache().getPlugins() )
	{
		std::cout << " - plugin: " << plugin->getIdentifier() << std::endl;
	}
	return 0;
}

