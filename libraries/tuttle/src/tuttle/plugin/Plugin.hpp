#ifndef _TUTTLE_PLUGIN_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PLUGIN_HPP_

#include <ofxsImageEffect.h>

#include <boost/preprocessor/stringize.hpp>
#include <string>

namespace tuttle {
namespace plugin {

/**
 * @brief Suppress 'warning: unused variable'
 */
void unusedString( const char* ) {}

/**
 * @brief Internal, use mAppendPluginFactory instead.
 */
template<class PluginFactory>
void appendPluginFactory( OFX::PluginFactoryArray& ids, const char* idString, const int versionMajor, const int versionMinor, const char* uniqueId )
{
	static const char* uniqueIdStr = uniqueId; //Make the uniqueId present in the final binary, even in release. OFXPLUGIN_fr.tuttle.myplug_1.0

	unusedString( uniqueIdStr );
	static PluginFactory p( idString, versionMajor, versionMinor );
	ids.push_back( &p );
}

/**
 * @brief Use this macro to declare your plugin in the mainEntry.cpp if you have plugins who don't have the same version in the bundle.
 */
#define mAppendPluginFactoryVersions( ids, ClassName, idString, versionMajor, versionMinor ) \
    tuttle::plugin::appendPluginFactory < ClassName > ( ids, idString, versionMajor, versionMinor, "OFXPLUGIN_" idString "_" BOOST_PP_STRINGIZE( versionMajor ) "." BOOST_PP_STRINGIZE( versionMinor ) );

/**
 * @brief Use this macro to declare your plugin in the mainEntry.cpp
 */
#define mAppendPluginFactory( ids, ClassName, idString ) \
    mAppendPluginFactoryVersions( ids, ClassName, idString, OFXPLUGIN_VERSION_MAJOR, OFXPLUGIN_VERSION_MINOR )
}
}

#endif
