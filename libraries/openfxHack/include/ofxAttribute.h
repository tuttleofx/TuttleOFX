#ifndef _ofxAttribute_h_
#define _ofxAttribute_h_

/*
 * Software License :
 *
 * Copyright (c) 2003-2009, The Open Effects Association Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ofxCore.h"
#include "ofxProperty.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief string value to the ::kOfxPropType property for all parameters */
//#define kOfxAttributeSuite "OfxAttributeSuite"

/** @brief string value on the ::kOfxPropType property for all parameter definitions (ie: the handle returned in describe) */
//#define kOfxTypeAttribute "OfxTypeAttribute"

/** @brief string value on the ::kOfxPropType property for all parameter instances */
//#define kOfxTypeAttributeInstance "OfxTypeAttributeInstance"

/**
 * @brief Blind declaration of an OFX param
 */
typedef struct OfxAttributeStruct* OfxAttributeHandle;

/**
 * @brief Blind declaration of an OFX parameter set
 */
typedef struct OfxAttributeSetStruct* OfxAttributeSetHandle;

/**
 * @defgroup ParamTypeDefines Parameter Type definitions
 * These strings are used to identify the type of the attribute.
 */
///@{

/** @brief String to identify an attribute as a parameter */
#define kOfxAttributeTypeParam "OfxAttributeTypeParam"
/** @brief String to identify an attribute as a clip */
#define kOfxAttributeTypeClip "OfxAttributeTypeClip"

/** @brief String that is the name of the standard OFX output attribute */
#define kOfxOutputAttributeName "Output"

/** @brief String that is the name of the standard OFX single source input attribute */
#define kOfxSimpleSourceAttributeName "Source"

///@}

/**
 * @file ofxAttribute.h
 * For more details go see @ref AttributesPage
 */

#ifdef __cplusplus
}
#endif

#endif
