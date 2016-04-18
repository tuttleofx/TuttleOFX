#define BOOST_TEST_MODULE properties_sezialization_tests
#include <tuttle/test/main.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/Core.hpp>

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

BOOST_AUTO_TEST_SUITE(properties_tests_suite02)

BOOST_AUTO_TEST_CASE(properties_serialization)
{
    using namespace std;
    using namespace tuttle::host;

#define testString_1_ro "testString_1_ro"
#define testString_2_rw "testString_2_rw"
#define testPointer_1_rw "testPointer_1_rw"
#define testDouble_2_ro "testDouble_2_ro"
#define testDouble_1_rw "testDouble_1_rw"
#define testInt_1_rw "testInt_1_rw"
#define testInt_2_rw "testInt_2_rw"
#define testInt_4_ro "testInt_4_ro"

    static const ofx::property::OfxhPropSpec testStuff[] = {
        /* name                                 type                   dim.   r/o    default value */
        {testString_1_ro, ofx::property::ePropTypeString, 1, true, "default_value"},
        {testString_2_rw, ofx::property::ePropTypeString, 2, false, "default_value"},
        {testPointer_1_rw, ofx::property::ePropTypePointer, 1, false, NULL},
        {testDouble_2_ro, ofx::property::ePropTypeDouble, 2, true, "0"},
        {testDouble_1_rw, ofx::property::ePropTypeDouble, 1, false, "0"},
        {testInt_1_rw, ofx::property::ePropTypeInt, 1, false, "0"},
        {testInt_2_rw, ofx::property::ePropTypeInt, 2, false, "8"},
        {testInt_4_ro, ofx::property::ePropTypeInt, 4, true, "0"},
        {0}};

    // init values
    ofx::property::OfxhSet testSet(testStuff);
    testSet.setStringProperty(testString_1_ro, "testcontext");
    testSet.setIntProperty(testInt_1_rw, true);
    int tab4Int[4] = {11, 22, 33, 44};
    testSet.setIntProperty(testInt_4_ro, tab4Int[0], 0);
    testSet.setIntProperty(testInt_4_ro, tab4Int[1], 1);
    testSet.setIntProperty(testInt_4_ro, tab4Int[2], 2);
    testSet.setIntProperty(testInt_4_ro, tab4Int[3], 3);
    double tab2Double[2] = {11, 22};
    testSet.setDoublePropertyN(testDouble_2_ro, tab2Double, 2);

    //	typedef boost::archive::binary_oarchive OArchive;
    //	typedef boost::archive::binary_iarchive IArchive;
    //	typedef boost::archive::text_oarchive OArchive;
    //	typedef boost::archive::text_iarchive IArchive;
    typedef boost::archive::xml_oarchive OArchive;
    typedef boost::archive::xml_iarchive IArchive;

    std::string testfile("test_properties_serialization.xml");
    BOOST_REQUIRE(testfile.size());

    std::ofstream ofsb(testfile.c_str());
    OArchive oArchive(ofsb);
    oArchive << BOOST_SERIALIZATION_NVP(testSet);
    ofsb.close();

    // new datas
    ofx::property::OfxhSet testSet2;

    std::ifstream ifsb(testfile.c_str());
    IArchive iArchive(ifsb);
    iArchive >> BOOST_SERIALIZATION_NVP(testSet2);
    ifsb.close();

    BOOST_CHECK(testSet == testSet2);

    BOOST_CHECK_EQUAL(0, std::remove(testfile.c_str()));
}

BOOST_AUTO_TEST_SUITE_END()
