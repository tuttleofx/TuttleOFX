#define BOOST_TEST_MODULE properties_tests
#include <tuttle/test/main.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/exceptions.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ImageEffectNode.hpp>

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <iostream>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(ofx_tests_suite01)

BOOST_AUTO_TEST_CASE(ofx_imageEffect_clones)
{
    using namespace std;
    using namespace tuttle::host;

    // get some plugins examples
    tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin =
        tuttle::host::core().getImageEffectPluginById("tuttle.pngreader");
    BOOST_CHECK(plugin != NULL);

    //	BOOST_CHECK_THROW( plugin->getProperties().fetchPointerProperty( kOfxPropInstanceData ), exception::LogicError );

    boost::scoped_ptr<tuttle::host::ImageEffectNode> ofxinst(
        dynamic_cast<tuttle::host::ImageEffectNode*>(plugin->createInstance(kOfxImageEffectContextReader)));
    ofxinst->setName("pluginTest");
    BOOST_CHECK(ofxinst->getProperties().fetchPointerProperty(kOfxPropInstanceData).getValue() != NULL);

    boost::scoped_ptr<tuttle::host::ImageEffectNode> ofxinstclone(ofxinst->clone());

    BOOST_CHECK_NE(ofxinst.get(), ofxinstclone.get());
    BOOST_CHECK((*ofxinst == *ofxinstclone));
    BOOST_CHECK(ofxinst->getProperties().fetchPointerProperty(kOfxPropInstanceData).getValue() != NULL);
    BOOST_CHECK(ofxinstclone->getProperties().fetchPointerProperty(kOfxPropInstanceData).getValue() != NULL);
    BOOST_CHECK(ofxinst->getProperties().fetchPointerProperty(kOfxPropInstanceData).getValue() !=
                ofxinstclone->getProperties().fetchPointerProperty(kOfxPropInstanceData).getValue());
}

BOOST_AUTO_TEST_SUITE_END()
