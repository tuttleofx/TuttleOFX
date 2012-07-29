#include <iostream>
#define BOOST_TEST_MODULE plugin_Jpeg
#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_Jpeg_load )

BOOST_AUTO_TEST_CASE( loading_openfx_plugins )
{
	TUTTLE_COUT( "-------- LOADING OPENFX PLUGINS --------" );
	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
	//TUTTLE_COUT( Core::instance().getImageEffectPluginCache() );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_SUITE_END()

