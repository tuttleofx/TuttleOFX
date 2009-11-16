#ifndef _ofxParamAPI_h_
#define _ofxParamAPI_h_

// TUTTLE_TODO

#include "ofxCore.h"
#include "ofxAttribute.h"
#include "ofxParam.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String used to label OFX Param manipulation Plug-ins
 *
 * Set the pluginApi member of the OfxPluginHeader inside any OfxImageEffectPluginStruct
 * to be this so that the host knows the plugin is an image effect.
 */
#define kOfxParamPluginApi "OfxParamPluginAPI"

/**
 * @brief The current version of the Param API
 */
#define kOfxParamPluginApiVersion 1






#ifdef __cplusplus
}
#endif


#endif


