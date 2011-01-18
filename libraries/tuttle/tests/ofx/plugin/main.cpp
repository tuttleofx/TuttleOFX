#define BOOST_TEST_MODULE imageeffectplugin_tests
#include <boost/test/unit_test.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/extended_type_info.hpp>

#include <fstream>
#include <iostream>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( plugin_serialization )

BOOST_AUTO_TEST_CASE( imageeffectplugin_serialization )
{
	using namespace std;
	using namespace tuttle::host;
	using namespace tuttle::host::ofx;
	using namespace tuttle::host::ofx::imageEffect;

	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
	OfxhImageEffectPlugin* plugin = Core::instance().getImageEffectPluginById( "fr.tuttle.invert" );

	//	typedef boost::archive::binary_oarchive OArchive;
	//	typedef boost::archive::binary_iarchive IArchive;
	//	typedef boost::archive::text_oarchive OArchive;
	//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;

	std::string testfile( "test_imageEffectPlugin_serialization.xml" );
	BOOST_REQUIRE( testfile.size() );

	{
		std::ofstream ofsb( testfile.c_str() );
		OArchive oArchive( ofsb );
		oArchive << BOOST_SERIALIZATION_NVP( plugin );
		ofsb.close();
	}

	// new datas
	OfxhImageEffectPlugin* plugin2 = NULL;

	{
		std::ifstream ifsb( testfile.c_str() );
		IArchive iArchive( ifsb );
		iArchive >> BOOST_SERIALIZATION_NVP( plugin2 );
		ifsb.close();
	}

	std::string testfile2( "test_imageEffectPlugin_serialization2.xml" );
	BOOST_REQUIRE( testfile2.size() );

	{
		std::ofstream ofsb2( testfile2.c_str() );
		OArchive oArchive2( ofsb2 );
		oArchive2 << BOOST_SERIALIZATION_NVP( plugin2 );
		ofsb2.close();
	}

	BOOST_CHECK( ( *plugin ) == ( *plugin2 ) );

}

BOOST_AUTO_TEST_SUITE_END()

