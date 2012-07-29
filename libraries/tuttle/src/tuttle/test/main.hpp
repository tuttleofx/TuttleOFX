#ifndef _TUTTLE_TEST_TEST_HPP_
#define _TUTTLE_TEST_TEST_HPP_

#include <boost/test/unit_test.hpp>

#include <tuttle/host/PreloadPlugins.hpp>

using namespace tuttle::host;
BOOST_GLOBAL_FIXTURE( PreloadPlugins );

#endif

