#ifndef _ofxParamAPI_h_
#define _ofxParamAPI_h_

#include "ofxCore.h"
#include "ofxAttribute.h"
#include "ofxParam.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String used to label OFX Param manipulation Plug-ins
 *
 * Set the pluginApi member of the OfxPluginHeader inside any OfxParamNodePluginStruct
 * to be this so that the host knows the plugin is an param node.
 */
#define kOfxParamPluginApi "OfxParamPluginAPI"

/**
 * @brief The current version of the Param API
 */
#define kOfxParamPluginApiVersion 1

/** @brief Blind declaration of an OFX param node
 */
typedef struct OfxParamNodeStruct* OfxParamNodeHandle;


/** @brief Used as a value for ::kOfxPropType on param node host handles */
#define kOfxTypeParameterNodeHost "OfxTypeParameterNodeHost"

/** @brief Used as a value for ::kOfxPropType on param node plugin handles */
#define kOfxTypeParameterNode "OfxTypeParameterNode"

/** @brief Used as a value for ::kOfxPropType on image effect instance handles  */
#define kOfxTypeParameterNodeInstance "OfxTypeParameterNodeInstance"



/**
 * @defgroup ActionsParamNode Actions Param Node
 */
///@{

#define kOfxParamNodeActionGetTimeDomain "OfxParamNodeActionGetTimeDomain"
/// GetTimeRangeNeeded is similar to the function GetFramesNeeded in ImageEffectNode
#define kOfxParamNodeActionGetTimeRangeNeeded "OfxParamNodeActionGetTimeRangeNeeded"
#define kOfxParamNodeActionProcess "OfxParamNodeActionProcess"
#define kOfxParamNodeActionBeginSequenceProcess "OfxParamNodeActionBeginSequenceProcess"
#define kOfxParamNodeActionEndSequenceProcess "OfxParamNodeActionEndSequenceProcess"

///@}

/** @brief the string that names image effect suites, passed to OfxHost::fetchSuite */
#define kOfxParamNodeSuite "OfxParamNodeSuite"


/** @brief The OFX suite for param manipulation
 *
 * This suite provides the functions needed by a plugin to defined and use a param manipulation plugin.
 */
typedef struct OfxParamNodeSuiteV1
{
	/** @brief Retrieves the property set for the given param node
	 *
	 * \arg paramNode   param node to get the property set for
	 * \arg propHandle    pointer to a the property set pointer, value is returned here
	 *
	 * The property handle is for the duration of the param node handle.
	 *
	 * @returns
	 * - ::kOfxStatOK       - the property set was found and returned
	 * - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
	 * - ::kOfxStatErrUnknown    - if the type is unknown
	 */
	OfxStatus ( *getPropertySet )( OfxParamNodeHandle  paramNode,
	                               OfxPropertySetHandle* propHandle );

	/** @brief Retrieves the parameter set for the given param node
	 *
	 * \arg paramNode   param node to get the property set for
	 * \arg paramSet     pointer to a the parameter set, value is returned here
	 *
	 * The param set handle is valid for the lifetime of the param node handle.
	 *
	 * @returns
	 * - ::kOfxStatOK       - the property set was found and returned
	 * - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
	 * - ::kOfxStatErrUnknown    - if the type is unknown
	 */
	OfxStatus ( *getParamSet )( OfxParamNodeHandle paramNode,
	                            OfxParamSetHandle*   paramSet );

	/** @brief Define a clip to the effect.
	 *
	 * \arg pluginHandle - the handle passed into 'describeInContext' action
	 * \arg name - unique name of the clip to define
	 * \arg propertySet - a property handle for the clip descriptor will be returned here
	 *
	 * This function defines a clip to a host, the returned property set is used to describe
	 * various aspects of the clip to the host. Note that this does not create a clip instance.
	 *
	 * \pre
	 * - we are inside the describe in context action.
	 *
	 * @returns
	 */
	OfxStatus ( *clipDefine )( OfxParamNodeHandle  paramNode,
	                           const char*           name,
	                           OfxPropertySetHandle* propertySet );

	/** @brief Get the propery handle of the named input clip in the given instance
	 *
	 * \arg paramNode - an instance handle to the plugin
	 * \arg name        - name of the clip, previously used in a clip define call
	 * \arg clip        - where to return the clip
	 * \arg propertySet  if not null, the descriptor handle for a parameter's property set will be placed here.
	 *
	 * The propertySet will have the same value as would be returned by OfxParamNodeSuiteV1::clipGetPropertySet
	 *
	 *  This return a clip handle for the given instance, note that this will \em not be the same as the
	 *  clip handle returned by clipDefine and will be distanct to clip handles in any other instance
	 *  of the plugin.
	 *
	 *  Not a valid call in any of the describe actions.
	 *
	 * \pre
	 * - create instance action called,
	 * - \e name passed to clipDefine for this context,
	 * - not inside describe or describe in context actions.
	 *
	 * \post
	 * - handle will be valid for the life time of the instance.
	 *
	 */
	OfxStatus ( *clipGetHandle )( OfxParamNodeHandle  paramNode,
	                              const char*           name,
	                              OfxImageClipHandle*   clip,
	                              OfxPropertySetHandle* propertySet );

	/** @brief Retrieves the property set for a given clip
	 *
	 * \arg clip          clip effect to get the property set for
	 * \arg propHandle    pointer to a the property set handle, value is returedn her
	 *
	 * The property handle is valid for the lifetime of the clip, which is generally the lifetime of the instance.
	 *
	 * @returns
	 * - ::kOfxStatOK       - the property set was found and returned
	 * - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
	 * - ::kOfxStatErrUnknown    - if the type is unknown
	 */
	OfxStatus ( *clipGetPropertySet )( OfxImageClipHandle    clip,
	                                   OfxPropertySetHandle* propHandle );

	/** @brief Get a handle for an image in a clip at the indicated time and indicated region
	 *
	 *  \arg clip  - the clip to extract the image from
	 *  \arg time        - time to fetch the image at
	 *  \arg region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
	 *                        this is in the \ref CanonicalCoordinates.
	 *  \arg imageHandle - property set containing the image's data
	 *
	 * An image is fetched from a clip at the indicated time for the given region and returned in the imageHandle.
	 *
	 * If the \e region parameter is not set to NULL, then it will be clipped to the clip's Region of Definition for the given time. The returned image will be \em at \em least as big as this region. If the region parameter is not set, then the region fetched will be at least the Region of Interest the effect has previously specified, clipped the clip's Region of Definition.
	 *
	 * If clipGetImage is called twice with the same parameters, then two separate image handles will be returned, each of which must be release. The underlying implementation could share image data pointers and use reference counting to maintain them.
	 *
	 * \pre
	 * - clip was returned by clipGetHandle
	 *
	 * \post
	 * - image handle is only valid for the duration of the action clipGetImage is called in
	 * - image handle to be disposed of by clipReleaseImage before the action returns
	 *
	 * @returns
	 * - ::kOfxStatOK - the image was successfully fetched and returned in the handle,
	 * - ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time and/or region, the plugin
	 *                 should continue operation, but assume the image was black and transparent.
	 * - ::kOfxStatErrBadHandle - the clip handle was invalid,
	 * - ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.
	 *
	 */
	OfxStatus ( *clipGetImage )( OfxImageClipHandle    clip,
	                             OfxTime               time,
	                             OfxRectD*             region,
	                             OfxPropertySetHandle* imageHandle );

	/** @brief Releases the image handle previously returned by clipGetImage
	 *
	 *
	 * \pre
	 * - imageHandle was returned by clipGetImage
	 *
	 * \post
	 * - all operations on imageHandle will be invalid
	 *
	 * @returns
	 * - ::kOfxStatOK - the image was successfully fetched and returned in the handle,
	 * - ::kOfxStatErrBadHandle - the image handle was invalid,
	 */
	OfxStatus ( *clipReleaseImage )( OfxPropertySetHandle imageHandle );

	/** @brief Returns the spatial region of definition of the clip at the given time
	 *
	 *  \arg clipHandle  - the clip to extract the image from
	 *  \arg time        - time to fetch the image at
	 *  \arg region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
	 *                        this is in the \ref CanonicalCoordinates.
	 *
	 * \pre
	 * - clipHandle was returned by clipGetHandle
	 *
	 * \post
	 * - bounds will be filled the RoD of the clip at the indicated time
	 *
	 * @returns
	 * - ::kOfxStatOK - the image was successfully fetched and returned in the handle,
	 * - ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time, the plugin
	 *                 should continue operation, but assume the image was black and transparent.
	 * - ::kOfxStatErrBadHandle - the clip handle was invalid,
	 * - ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.
	 *
	 *
	 */
	OfxStatus ( *clipGetRegionOfDefinition )( OfxImageClipHandle clip,
	                                          OfxTime            time,
	                                          OfxRectD*          bounds );

	/** @brief Returns whether to abort processing or not.
	 *
	 *  \arg paramNode  - instance of the param node
	 *
	 * A host may want to signal to a plugin that it should stop whatever rendering it is doing and start again.
	 * Generally this is done in interactive threads in response to users tweaking some parameter.
	 *
	 * This function indicates whether a plugin should stop whatever processing it is doing.
	 *
	 * @returns
	 * - 0 if the effect should continue whatever processing it is doing
	 * - 1 if the effect should abort whatever processing it is doing
	 */
	int ( *abort )( OfxParamNodeHandle paramNode );

	/** @brief Allocate memory from the host's image memory pool
	 *
	 * \arg instanceHandle  - effect instance to associate with this memory allocation, may be NULL.
	 * \arg nBytes          - the number of bytes to allocate
	 * \arg memoryHandle    - pointer to the memory handle where a return value is placed
	 *
	 * Memory handles allocated by this should be freed by OfxParamNodeSuiteV1::imageMemoryFree.
	 * To access the memory behind the handle you need to call  OfxParamNodeSuiteV1::imageMemoryLock.
	 *
	 * See \ref ParamNodesMemoryAllocation.
	 *
	 * @returns
	 * - kOfxStatOK if all went well, a valid memory handle is placed in \e memoryHandle
	 * - kOfxStatErrBadHandle if instanceHandle is not valid, memoryHandle is set to NULL
	 * - kOfxStatErrMemory if there was not enough memory to satisfy the call, memoryHandle is set to NULL
	 */
	OfxStatus ( *imageMemoryAlloc )( OfxParamNodeHandle  instanceHandle,
	                                 size_t                nBytes,
	                                 OfxImageMemoryHandle* memoryHandle );

	/** @brief Frees a memory handle and associated memory.
	 *
	 * \arg memoryHandle - memory handle returned by imageMemoryAlloc
	 *
	 * This function frees a memory handle and associated memory that was previously allocated via OfxParamNodeSuiteV1::imageMemoryAlloc
	 *
	 * If there are outstanding locks, these are ignored and the handle and memory are freed anyway.
	 *
	 * See \ref ParamNodesMemoryAllocation.
	 *
	 * @returns
	 * - kOfxStatOK if the memory was cleanly deleted
	 * - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxParamNodeSuiteV1::imageMemoryAlloc
	 */
	OfxStatus ( *imageMemoryFree )( OfxImageMemoryHandle memoryHandle );

	/** @brief Lock the memory associated with a memory handle and make it available for use.
	 *
	 * \arg memoryHandle - memory handle returned by imageMemoryAlloc
	 * \arg returnedPtr - where to the pointer to the locked memory
	 *
	 * This function locks them memory associated with a memory handle and returns a pointer to it. The memory will be 16 byte aligned, to allow use of vector operations.
	 *
	 * Note that memory locks and unlocks nest.
	 *
	 * After the first lock call, the contents of the memory pointer to by \e returnedPtr is undefined. All subsequent calls to lock will return memory with the same contents as  the previous call.
	 *
	 * Also, if unlocked, then relocked, the memory associated with a memory handle may be at a different address.
	 *
	 * See also OfxParamNodeSuiteV1::imageMemoryUnlock and \ref ParamNodesMemoryAllocation.
	 *
	 * @returns
	 * - kOfxStatOK if the memory was locked, a pointer is placed in \e returnedPtr
	 * - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxParamNodeSuiteV1::imageMemoryAlloc, null is placed in \e *returnedPtr
	 * - kOfxStatErrMemory if there was not enough memory to satisfy the call, \e *returnedPtr is set to NULL
	 */
	OfxStatus ( *imageMemoryLock )( OfxImageMemoryHandle memoryHandle,
	                                void**               returnedPtr );

	/** @brief Unlock allocated image data
	 *
	 * \arg allocatedData - pointer to memory previously returned by OfxParamNodeSuiteV1::imageAlloc
	 *
	 * This function unlocks a previously locked memory handle. Once completely unlocked, memory associated with a memoryHandle is no longer available for use. Attempting to use it results in undefined behaviour.
	 *
	 * Note that locks and unlocks nest, and to fully unlock memory you need to match the count of locks placed upon it.
	 *
	 * Also note, if you unlock a completely unlocked handle, it has no effect (ie: the lock count can't be negative).
	 *
	 * If unlocked, then relocked, the memory associated with a memory handle may be at a different address, however the contents will remain the same.
	 *
	 * See also OfxParamNodeSuiteV1::imageMemoryLock and \ref ParamNodesMemoryAllocation.
	 *
	 * @returns
	 * - kOfxStatOK if the memory was unlocked cleanly,
	 * - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxParamNodeSuiteV1::imageMemoryAlloc, null is placed in \e *returnedPtr
	 */
	OfxStatus ( *imageMemoryUnlock )( OfxImageMemoryHandle memoryHandle );

} OfxParamNodeSuiteV1;



#ifdef __cplusplus
}
#endif

#endif

