#ifndef _ofxClip_h_
#define _ofxClip_h_

#include "ofxCore.h"

#ifdef __cplusplus
extern "C" {
#endif

// TUTTLE_TODO
// out what can be output from ofxImageEffect.h

/** @brief Used as a value for ::kOfxPropType on image effect clips */
#define kOfxTypeClip "OfxTypeClip" // move from ofxImageEffect.h

/**
 * @brief The type of a clip.
 *
 *  - Type - C string X 1
 *  - Property Set - plugin clip descriptor (read only) and instance (read only)
 *
 * This string will be set to the type that the parameter was create with.
 */
#define kOfxClipPropType "OfxClipPropType"

/** @brief String to identify a clip as an imageClip */
#define kOfxClipTypeImage "OfxClipTypeImage"

#ifdef __cplusplus
}
#endif

#endif

