#include <tuttle/common/utils/global.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/foreach.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	boost::shared_ptr<tuttle::common::formatters::Formatter> formatter( tuttle::common::formatters::Formatter::get() );
	boost::shared_ptr<tuttle::common::Color>                 color( tuttle::common::Color::get() );
	formatter->init_logging();
	color->disable();
	
	using namespace tuttle::host;
	core().preload();

	BOOST_FOREACH( ofx::OfxhPlugin* plugin, core().getPluginCache().getPlugins() )
	{
		TUTTLE_LOG_INFO( " - plugin: " << plugin->getIdentifier() );
	}
	return 0;
}

