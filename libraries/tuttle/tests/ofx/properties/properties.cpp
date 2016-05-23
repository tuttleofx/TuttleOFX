#define BOOST_TEST_MODULE properties_properties_tests
#include <tuttle/test/main.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/Core.hpp>

#include <iostream>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(properties_tests_suite01)

BOOST_AUTO_TEST_CASE(properties_get_and_set)
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

    // tests
    BOOST_CHECK_EQUAL(testSet.getStringProperty(testString_1_ro), "testcontext");
    BOOST_CHECK_EQUAL(testSet.getStringProperty(testString_2_rw, 0), "default_value");
    BOOST_CHECK_EQUAL(testSet.getStringProperty(testString_2_rw, 1), "default_value");

    BOOST_CHECK_EQUAL(testSet.getIntProperty(testInt_1_rw), true);
    BOOST_CHECK_EQUAL(testSet.getIntProperty(testInt_2_rw, 0), 8);
    BOOST_CHECK_EQUAL(testSet.getIntProperty(testInt_2_rw, 1), 8);

    for(unsigned int i = 0; i < 4; ++i)
        BOOST_CHECK_EQUAL(testSet.getIntProperty(testInt_4_ro, i), tab4Int[i]);

    int tabGet4Int[4] = {0, 0, 0, 0};
    testSet.getIntPropertyN(testInt_4_ro, tabGet4Int, 4);
    for(unsigned int i = 0; i < 4; ++i)
        BOOST_CHECK_EQUAL(tabGet4Int[i], tab4Int[i]);

    for(unsigned int i = 0; i < 2; ++i)
        BOOST_CHECK_EQUAL(testSet.getDoubleProperty(testDouble_2_ro, i), tab2Double[i]);

    BOOST_CHECK_THROW(testSet.getStringProperty("unexisting_properties"), ofx::OfxhException);
    BOOST_CHECK_THROW(testSet.getIntProperty(testString_2_rw), std::bad_cast);

    // Assignment and comparison tests
    ofx::property::OfxhSet testBSet(testSet);
    BOOST_CHECK(testBSet == testSet);

    testBSet.setIntProperty(testInt_4_ro, tab4Int[0] + 1, 0);
    BOOST_CHECK(testBSet != testSet);

    testBSet.setIntProperty(testInt_4_ro, tab4Int[0], 0);
    BOOST_CHECK(testBSet == testSet);

    ofx::property::OfxhSet testCSet;
    testCSet = testBSet;
    BOOST_CHECK(testCSet == testBSet);

    testCSet.setIntProperty(testInt_4_ro, tab4Int[0] + 11, 0);
    BOOST_CHECK(testCSet != testBSet);
    testBSet.setIntProperty(testInt_4_ro, tab4Int[0] + 11, 0);
    BOOST_CHECK(testCSet == testBSet);

    //	bool sequential = descriptor.getProperties().getIntProperty( kOfxImageEffectInstancePropSequentialRender ) != 0;
}

BOOST_AUTO_TEST_SUITE_END()
