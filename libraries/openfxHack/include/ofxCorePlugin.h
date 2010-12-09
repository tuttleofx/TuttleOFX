/*
 *  Contains the core architectural structures and function definitions for plug-in mechanism.
 *  This code is a subset of ofxCore.h from OpenFX plug-in mechanism ( http://openfx.sourceforge.net/ )
 */

#ifndef _COREPLUGIN_H_
#define _COREPLUGIN_H_

#include <stddef.h> // for size_t
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Platform independent export macro.
 *
 * This macro is to be used before any symbol that is to be
 * exported from a plug-in. This is OS/compiler dependent.
 */
#if ( defined( WIN32 ) || defined( WIN64 ) || defined( _WIN32 ) || defined( _WIN64 ) || defined( __WINDOWS__ ) || defined( __TOS_WIN__ ) || defined( __WIN32__ ) )
 #define OfxExport extern __declspec( dllexport )
#elif defined( __GNUC__ )     // Add compiler definition here...
 #if __GNUC__ - 0 > 3 || ( __GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 > 2 )
  #define OfxExport __attribute__ ( ( visibility( "default" ) ) )
 #else
  #define OfxExport
  #warning "OfxExport not set because of a too old gcc version. The plug-in may not compile with the option -fvisible=hidden."
 #endif
#else
 #error "OfxExport not defined for this compiler..."
#endif

/** @brief Blind data structure to manipulate sets of properties through */
typedef struct OfxPropertySetStruct* OfxPropertySetHandle;

/** @brief OFX status return type */
typedef int OfxStatus;

/** @brief Generic host structure passed to OfxPlugin::setHost function

    This structure contains what is needed by a plug-in to bootstrap it's connection
    to the host.
 */
typedef struct OfxHost
{
	/** @brief Global handle to the host. Extract relevant host properties from this.
	    This pointer will be valid while the binary containing the plug-in is loaded.
	 */
	OfxPropertySetHandle host;

	/** @brief The function which the plug-in uses to fetch suites from the host.

	    \arg \e host          - the host the suite is being fetched from this \em must be the \e host member of the OfxHost struct containing fetchSuite.
	    \arg \e suiteName     - ASCII string labelling the host supplied API
	    \arg \e suiteVersion  - version of that suite to fetch

	    Any API fetched will be valid while the binary containing the plug-in is loaded.

	    Repeated calls to fetchSuite with the same parameters will return the same pointer.

	    returns
	       - NULL if the API is unknown (either the api or the version requested),
	   - pointer to the relevant API if it was found
	 */
	void*( *fetchSuite )( OfxPropertySetHandle host, const char* suiteName, int suiteVersion );
} OfxHost;

/** @brief Entry point for plug-ins

   \arg \e action   - ASCII c string indicating which action to take
   \arg \e instance - object to which action should be applied, this will need to be cast to the appropriate blind data type depending on the \e action
   \arg \e inData   - handle that contains action specific properties
   \arg \e outData  - handle where the plug-in should set various action specific properties

   This is how the host generally communicates with a plug-in. Entry points are used to pass messages
   to various objects used within OFX. The main use is within the OfxPlugin struct.

   The exact set of actions is determined by the plug-in API that is being implemented, however all plug-ins
   can perform several actions. For the list of actions consult \ref ActionsAll.
 */
typedef  OfxStatus ( OfxPluginEntryPoint )( const char* action, const void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs );

/** @brief The structure that defines a plug-in to a host.
 *
 * This structure is the first element in any plug-in structure
 * using the OFX plug-in architecture. By examining it's members
 * a host can determine the API that the plug-in implements,
 * the version of that API, it's name and version.
 *
 * For details see \ref Architecture.
 *
 */
typedef struct OfxPlugin
{
	/** Defines the type of the plug-in, this will tell the host what the plug-in does. e.g.: an image
	    effects plug-in would be a "OfxImageEffectPlugin"
	 */
	const char* pluginApi;

	/** Defines the version of the pluginApi that this plug-in implements */
	int apiVersion;

	/** String that uniquely labels the plug-in among all plug-ins that implement an API.
	    It need not necessarily be human sensible, however the preference is to use reverse
	    internet domain name of the developer, followed by a '.' then by a name that represents
	    the plug-in.. It must be a legal ASCII string and have no whitespace in the
	    name and no non printing chars.
	    For example "uk.co.somesoftwarehouse.myPlugin"
	 */
	const char* pluginIdentifier;

	/** Major version of this plug-in, this gets incremented when backwards compatibility is broken. */
	unsigned int pluginVersionMajor;

	/**  Major version of this plug-in, this gets incremented when software is changed,
	     but does not break backwards compatibility. */
	unsigned int pluginVersionMinor;

	/** @brief Function the host uses to connect the plug-in to the host's api fetcher

	    \arg \e fetchApi - pointer to host's API fetcher

	    Mandatory function.

	    The very first function called in a plug-in. The plug-in \em must \em not call any OFX functions within this, it must only set it's local copy of the host pointer.

	    \pre
	      - nothing else has been called

	    \post
	      - the pointer suite is valid until the plug-in is unloaded
	 */
	void ( * setHost )( OfxHost* host );

	/** @brief Main entry point for plug-ins

	   Mandatory function.

	   The exact set of actions is determined by the plug-in API that is being implemented, however all plug-ins
	   can perform several actions. For the list of actions consult \ref ActionsAll.

	   Preconditions
	    - setHost has been called
	 */
	OfxPluginEntryPoint* mainEntry;
} OfxPlugin;

/**
   \defgroup ActionsAll OFX Actions

   These are the actions passed to a plug-in's 'main' function
 */
/*@{*/

/** @brief Action called just after a plug-in has been loaded, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralLoad */
#define  kOfxActionLoad "OfxActionLoad"

/** @brief Action called to have a plug-in describe itself to the host, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralDescribe */
#define kOfxActionDescribe "OfxActionDescribe"

/** @brief Action called just before a plug-in is unloaded, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralUnload */
#define kOfxActionUnload "OfxActionUnload"

/** @brief Action called just after an instance has been created \ref ArchitectureMainFunction and \ref ActionsGeneralCreateInstance  */
#define kOfxActionCreateInstance        "OfxActionCreateInstance"

/** @brief Action called just before an instance is destroyed and \ref ActionsGeneralDestroyInstance */
#define kOfxActionDestroyInstance       "OfxActionDestroyInstance"

/*@}*/

/** @brief Returns the 'nth' plug-in implemented inside a binary
 *
 * Returns a pointer to the 'nth' plug-in implemented in the binary. A function of this type
 * must be implemented in and exported from each plug-in binary.
 */
OfxExport OfxPlugin* OfxGetPlugin( int nth );

/** @brief Defines the number of plug-ins implemented inside a binary
 *
 * A host calls this to determine how many plug-ins there are inside
 * a binary it has loaded. A function of this type
 * must be implemented in and exported from each plug-in binary.
 */
OfxExport int OfxGetNumberOfPlugins( void );

/*@{*/

/** @brief Status code indicating all was fine */
#define kOfxStatOK 0

/** @brief Status error code for a failed operation */
#define kOfxStatFailed  ( (int)1 )

/** @brief Status error code for a fatal error

   Only returned in the case where the plug-in or host cannot continue to function and needs to be restarted.
 */
#define kOfxStatErrFatal ( (int)2 )

/** @brief Status error code for an operation on or request for an unknown object */
#define kOfxStatErrUnknown ( (int)3 )

/** @brief Status error code returned by plug-ins when they are missing host functionality, either an API or some optional functionality (eg: custom params).

    Plug-Ins returning this should post an appropriate error message stating what they are missing.
 */
#define kOfxStatErrMissingHostFeature ( (int) 4 )

/** @brief Status error code for an unsupported feature/operation */
#define kOfxStatErrUnsupported ( (int) 5 )

/** @brief Status error code for an operation attempting to create something that exists */
#define kOfxStatErrExists  ( (int) 6 )

/** @brief Status error code for an incorrect format */
#define kOfxStatErrFormat ( (int) 7 )

/** @brief Status error code indicating that something failed due to memory shortage */
#define kOfxStatErrMemory  ( (int) 8 )

/** @brief Status error code for an operation on a bad handle */
#define kOfxStatErrBadHandle ( (int) 9 )

/** @brief Status error code indicating that a given index was invalid or unavailable */
#define kOfxStatErrBadIndex ( (int)10 )

/** @brief Status error code indicating that something failed due an illegal value */
#define kOfxStatErrValue ( (int) 11 )

/** @brief OfxStatus returned indicating a 'yes' */
#define kOfxStatReplyYes ( (int) 12 )

/** @brief OfxStatus returned indicating a 'no' */
#define kOfxStatReplyNo ( (int) 13 )

/** @brief OfxStatus returned indicating that a default action should be performed */
#define kOfxStatReplyDefault ( (int) 14 )

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_HPP_ */
