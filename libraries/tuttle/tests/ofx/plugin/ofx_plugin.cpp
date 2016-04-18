#define BOOST_TEST_MODULE imageeffectplugin_tests
#include <tuttle/test/main.hpp>

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
#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(plugin_serialization)

BOOST_AUTO_TEST_CASE(imageeffectplugin_serialization)
{
    using namespace std;
    using namespace tuttle::host;
    using namespace tuttle::host::ofx;
    using namespace tuttle::host::ofx::imageEffect;

    OfxhImageEffectPlugin* plugin = core().getImageEffectPluginById("tuttle.invert");

    //	typedef boost::archive::binary_oarchive OArchive;
    //	typedef boost::archive::binary_iarchive IArchive;
    //	typedef boost::archive::text_oarchive OArchive;
    //	typedef boost::archive::text_iarchive IArchive;
    typedef boost::archive::xml_oarchive OArchive;
    typedef boost::archive::xml_iarchive IArchive;

    //// Write "tuttle.invert" plugin into a cache file
    const std::string testfile =
        (core().getPreferences().buildTuttleTestPath() / "test_imageEffectPlugin_serialization.xml").string();
    TUTTLE_LOG_VAR(TUTTLE_TRACE, testfile);
    BOOST_REQUIRE(testfile.size());

    {
        std::ofstream ofsb(testfile.c_str());
        OArchive oArchive(ofsb);
        oArchive << BOOST_SERIALIZATION_NVP(plugin);
        ofsb.close();
    }

    BOOST_CHECK(boost::filesystem::exists(testfile));

    //// Read "tuttle.invert" plugin from the cache file
    OfxhImageEffectPlugin* plugin2 = NULL;

    {
        std::ifstream ifsb(testfile.c_str());
        IArchive iArchive(ifsb);
        iArchive >> BOOST_SERIALIZATION_NVP(plugin2);
        ifsb.close();
    }
    BOOST_CHECK(boost::filesystem::exists(testfile));
    boost::filesystem::remove(testfile);

    //// The new plugin re-created from the cache file is the same than the original
    BOOST_CHECK((*plugin) == (*plugin2));

    // Rewrite "tuttle.invert" plugin into another cache file
    const std::string testfile2 =
        (core().getPreferences().buildTuttleTestPath() / "test_imageEffectPlugin_serialization2.xml").string();
    TUTTLE_LOG_VAR(TUTTLE_TRACE, testfile2);
    BOOST_REQUIRE(testfile2.size());

    {
        std::ofstream ofsb2(testfile2.c_str());
        OArchive oArchive2(ofsb2);
        oArchive2 << BOOST_SERIALIZATION_NVP(plugin2);
        ofsb2.close();
    }

    BOOST_CHECK(boost::filesystem::exists(testfile2));
    boost::filesystem::remove(testfile2);
}

BOOST_AUTO_TEST_SUITE_END()
