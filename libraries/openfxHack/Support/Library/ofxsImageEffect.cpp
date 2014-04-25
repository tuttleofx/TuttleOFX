/*
* OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
* Copyright (C) 2004-2007 The Open Effects Association Ltd
* Author Bruno Nicoletti bruno@thefoundry.co.uk
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its
* contributors may be used to endorse or promote products derived from this
* software without specific prior written permission.
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
*
* The Open Effects Association Ltd
* 1 Wardour St
* London W1D 6PA
* England
*
*
*/

/** @brief This file contains code that skins the ofx effect suite */

#include "ofxsSupportPrivate.h"
#include "ofxsUtilities.h"

#include <tuttle/common/utils/backtrace.hpp>
#include <tuttle/common/exceptions.hpp>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <algorithm> // for find
#include <iostream>
#include <sstream>
#include <string>

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {

FramesNeededSetter::~FramesNeededSetter() {}
RegionOfInterestSetter::~RegionOfInterestSetter() {}


// globals to keep consistent data structures around.
OFX::PluginFactoryArray plugIDs;
//Put it all into a map, so we know when to delete what!
struct OfxPlugInfo
{
	OfxPlugInfo()
	: _factory(NULL)
	, _plug(NULL)
	{}
	OfxPlugInfo( OFX::PluginFactory* f, OfxPlugin* p )
	: _factory( f )
	, _plug( p )
	{}
	OFX::PluginFactory* _factory;
	OfxPlugin* _plug;
};

typedef std::map<std::string, OfxPlugInfo> OfxPlugInfoMap;
typedef std::vector<OfxPlugin*> OfxPluginArray;

namespace Private {
OfxPlugInfoMap plugInfoMap;
OfxPluginArray ofxPlugs;

/** @brief the global host description */
ImageEffectHostDescription gHostDescription;
bool gHostDescriptionHasInit = false;


/** @brief Keeps count of how many times load/unload have been called */
int gLoadCount = 0;
bool gHasInit = false;

// Suite and host pointers
OfxHost* gHost                      = NULL;
OfxImageEffectSuiteV1* gEffectSuite = NULL;
OfxPropertySuiteV1* gPropSuite      = NULL;
OfxInteractSuiteV1* gInteractSuite  = NULL;
OfxParameterSuiteV1* gParamSuite    = NULL;
OfxMemorySuiteV1* gMemorySuite      = NULL;
OfxMultiThreadSuiteV1* gThreadSuite = NULL;
OfxMessageSuiteV1* gMessageSuite    = NULL;
OfxProgressSuiteV1* gProgressSuite  = NULL;
OfxTimeLineSuiteV1* gTimeLineSuite  = NULL;
OfxParametricParameterSuiteV1* gParametricParameterSuite = NULL;
NukeOfxCameraSuiteV1* gCameraParameterSuite = NULL;

// @brief the set of descriptors, one per context used by kOfxActionDescribeInContext,
//'eContextNone' is the one used by the kOfxActionDescribe
EffectDescriptorMap gEffectDescriptors;
} // namespace Private


ImageEffectHostDescription* getImageEffectHostDescription()
{
	if( Private::gHostDescriptionHasInit )
	return &Private::gHostDescription;
	return NULL;
}


/** @brief map a std::string to a context */
EContext mapContextStringToEnum( const std::string& s )
{
	if( s == kOfxImageEffectContextGenerator )
	return eContextGenerator;
	if( s == kOfxImageEffectContextFilter )
	return eContextFilter;
	if( s == kOfxImageEffectContextTransition )
	return eContextTransition;
	if( s == kOfxImageEffectContextPaint )
	return eContextPaint;
	if( s == kOfxImageEffectContextGeneral )
	return eContextGeneral;
	if( s == kOfxImageEffectContextRetimer )
	return eContextRetimer;
	if( s == kOfxImageEffectContextReader )
	return eContextReader;
	if( s == kOfxImageEffectContextWriter )
	return eContextWriter;
	OFX::Log::error( true, "Unknown image effect context '%s'", s.c_str() );
	BOOST_THROW_EXCEPTION( std::invalid_argument( s ) );
	return eContextGeneral;
}

/** @brief map a std::string to a context */
const std::string mapContextEnumToString( const EContext s )
{
	switch( s )
	{
	case eContextGenerator:
		return kOfxImageEffectContextGenerator;
	case eContextFilter:
		return kOfxImageEffectContextFilter;
	case eContextTransition:
		return kOfxImageEffectContextTransition;
	case eContextPaint:
		return kOfxImageEffectContextPaint;
	case eContextGeneral:
		return kOfxImageEffectContextGeneral;
	case eContextRetimer:
		return kOfxImageEffectContextRetimer;
	case eContextReader:
		return kOfxImageEffectContextReader;
	case eContextWriter:
		return kOfxImageEffectContextWriter;
	case eContextNone:
		return "ContextNone...";
	}
	OFX::Log::error( true, "Unknown image effect context enum '%d'", (int)s );
	BOOST_THROW_EXCEPTION( std::invalid_argument( "Unknown image effect context enum." ) );
	return "";
}

const std::string mapMessageTypeEnumToString( const OFX::Message::EMessageType type )
{
	if( type == OFX::Message::eMessageFatal )
	return kOfxMessageFatal;
	else if( type == OFX::Message::eMessageError )
	return kOfxMessageError;
	else if( type == OFX::Message::eMessageMessage )
	return kOfxMessageMessage;
	else if( type == OFX::Message::eMessageLog )
	return kOfxMessageLog;
	else if( type == OFX::Message::eMessageQuestion )
	return kOfxMessageQuestion;
	OFX::Log::error( true, "Unknown message type enum '%d'", type );
	return 0;
}

OFX::Message::EMessageReply mapMessageReplyStatusToEnum( const OfxStatus stat )
{
	if( stat == kOfxStatOK )
	return OFX::Message::eMessageReplyOK;
	else if( stat == kOfxStatReplyYes )
	return OFX::Message::eMessageReplyYes;
	else if( stat == kOfxStatReplyNo )
	return OFX::Message::eMessageReplyNo;
	else if( stat == kOfxStatFailed )
	return OFX::Message::eMessageReplyFailed;
	OFX::Log::error( true, "Unknown message reply status value '%d'", stat );
	return OFX::Message::eMessageReplyFailed;
}

OfxStatus mapMessageReplyEnumToStatus( const OFX::Message::EMessageReply stat )
{
	switch( stat )
	{
	case OFX::Message::eMessageReplyOK:
		return kOfxStatOK;
	case OFX::Message::eMessageReplyYes:
		return kOfxStatReplyYes;
	case OFX::Message::eMessageReplyNo:
		return kOfxStatReplyNo;
	case OFX::Message::eMessageReplyFailed:
		return kOfxStatFailed;
	}
	OFX::Log::error( true, "Unknown message reply status enum '%d'", stat );
	return kOfxStatFailed;
}

/** @brief map a std::string to a context */
InstanceChangeReason mapInstanceChangedReasonStringToEnum( const std::string& s )
{
	if( s == kOfxChangePluginEdited )
	return eChangePluginEdit;
	if( s == kOfxChangeUserEdited )
	return eChangeUserEdit;
	if( s == kOfxChangeTime )
	return eChangeTime;
	OFX::Log::error( true, "Unknown instance changed reason '%s'", s.c_str() );
	BOOST_THROW_EXCEPTION( std::invalid_argument( s ) );
	return eChangePluginEdit;
}

/** @brief turns a bit depth string into and enum */
EBitDepth mapBitDepthStringToEnum( const std::string& str )
{
	if( str == kOfxBitDepthByte )
	{
	return eBitDepthUByte;
	}
	else if( str == kOfxBitDepthShort )
	{
	return eBitDepthUShort;
	}
	else if( str == kOfxBitDepthFloat )
	{
	return eBitDepthFloat;
	}
	else if( str == kOfxBitDepthNone )
	{
	return eBitDepthNone;
	}
	else
	{
	return eBitDepthCustom;
	}
}

const std::string mapBitDepthEnumToString( const EBitDepth e )
{
	switch(e)
	{
	case eBitDepthUByte:
		return kOfxBitDepthByte;
	case eBitDepthUShort:
		return kOfxBitDepthShort;
	case eBitDepthFloat:
		return kOfxBitDepthFloat;
	case eBitDepthNone:
		return kOfxBitDepthNone;
	case eBitDepthCustom:
		return "eBitDepthCustom";
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( "BitDepth enum: " + boost::lexical_cast<std::string>(e) ) );
	return kOfxBitDepthNone;
}

/** @brief turns a pixel component string into and enum */
EPixelComponent mapPixelComponentStringToEnum( const std::string& str )
{
	if( str == kOfxImageComponentRGBA )
	{
	return ePixelComponentRGBA;
	}
	else if( str == kOfxImageComponentRGB )
	{
	return ePixelComponentRGB;
	}
	else if( str == kOfxImageComponentAlpha )
	{
	return ePixelComponentAlpha;
	}
	else if( str == kOfxImageComponentNone )
	{
	return ePixelComponentNone;
	}
	else
	{
	return ePixelComponentCustom;
	}
}

std::string mapPixelComponentEnumToString( const EPixelComponent e )
{
	switch(e)
	{
	case ePixelComponentRGBA:
		return kOfxImageComponentRGBA;
	case ePixelComponentRGB:
		return kOfxImageComponentRGB;
	case ePixelComponentAlpha:
		return kOfxImageComponentAlpha;
	case ePixelComponentNone:
		return kOfxImageComponentNone;
	case ePixelComponentCustom:
		return "ePixelComponentCustom";
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( "EPixelComponent: " + boost::lexical_cast<std::string>(e) ) );
}

/** @brief turns a premultiplication string into and enum */
EPreMultiplication mapPreMultiplicationStringToEnum( const std::string& str )
{
	if( str == kOfxImageOpaque )
	{
	return eImageOpaque;
	}
	else if( str == kOfxImagePreMultiplied )
	{
	return eImagePreMultiplied;
	}
	else if( str == kOfxImageUnPreMultiplied )
	{
	return eImageUnPreMultiplied;
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( str ) );
	return eImageOpaque;
}

std::string mapPreMultiplicationEnumToString( const EPreMultiplication e )
{
	switch( e )
	{
	case eImageOpaque:
		return kOfxImageOpaque;
	case eImagePreMultiplied:
		return kOfxImagePreMultiplied;
	case eImageUnPreMultiplied:
		return kOfxImageUnPreMultiplied;
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( "PreMultiplicationEnum: " + boost::lexical_cast<std::string>(e) ) );
	return "";
}

/** @brief turns a field string into and enum */
EField mapFieldStringToEnum( const std::string& str )
{
	if( str == kOfxImageFieldNone )
	{
	return eFieldNone;
	}
	else if( str == kOfxImageFieldBoth )
	{
	return eFieldBoth;
	}
	else if( str == kOfxImageFieldLower )
	{
	return eFieldLower;
	}
	else if( str == kOfxImageFieldUpper )
	{
	return eFieldUpper;
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( str ) );
	return eFieldNone;
}

std::string mapFieldEnumToString( const EField e )
{
	switch( e )
	{
	case eFieldNone:
		return kOfxImageFieldNone;
	case eFieldBoth:
		return kOfxImageFieldBoth;
	case eFieldLower:
		return kOfxImageFieldLower;
	case eFieldUpper:
		return kOfxImageFieldUpper;
	}
	BOOST_THROW_EXCEPTION( std::invalid_argument( boost::lexical_cast<std::string>(e) ) );
	return "";
}

////////////////////////////////////////////////////////////////////////////////
// clip descriptor

/** @brief hidden constructor */
ClipDescriptor::ClipDescriptor( const std::string& name, OfxPropertySetHandle props )
	: _clipName( name ),
	_clipProps( props )
{
	OFX::Validation::validateClipDescriptorProperties( props );
}

/** @brief set the label properties */
void ClipDescriptor::setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel )
{
	_clipProps.propSetString( kOfxPropLabel, label );
	_clipProps.propSetString( kOfxPropShortLabel, shortLabel, false );
	_clipProps.propSetString( kOfxPropLongLabel, longLabel, false );
}

/** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
void ClipDescriptor::setFieldExtraction( EFieldExtraction v )
{
	switch( v )
	{
	case eFieldExtractBoth:
		_clipProps.propSetString( kOfxImageClipPropFieldExtraction, kOfxImageFieldBoth );
		break;

	case eFieldExtractSingle:
		_clipProps.propSetString( kOfxImageClipPropFieldExtraction, kOfxImageFieldSingle );
		break;

	case eFieldExtractDoubled:
		_clipProps.propSetString( kOfxImageClipPropFieldExtraction, kOfxImageFieldDoubled );
		break;
	}
}

/** @brief set which components are supported, defaults to none set, this must be called at least once! */
void ClipDescriptor::addSupportedComponent( EPixelComponent v )
{
	int n = _clipProps.propGetDimension( kOfxImageEffectPropSupportedComponents );

	switch( v )
	{
	case ePixelComponentRGBA:
		_clipProps.propSetString( kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGBA, n );
		break;
	case ePixelComponentRGB:
		_clipProps.propSetString( kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGB, n );
		break;
	case ePixelComponentAlpha:
		_clipProps.propSetString( kOfxImageEffectPropSupportedComponents, kOfxImageComponentAlpha, n );
		break;
	case ePixelComponentCustom:
		break;
	case ePixelComponentNone:
		_clipProps.propSetString( kOfxImageEffectPropSupportedComponents, kOfxImageComponentNone, n );
		break;
	}
}

/** @brief set which components are supported, defaults to none set, this must be called at least once! */
void ClipDescriptor::addSupportedComponent( const std::string& comp )
{
	int n = _clipProps.propGetDimension( kOfxImageEffectPropSupportedComponents );

	_clipProps.propSetString( kOfxImageEffectPropSupportedComponents, comp, n );

}

/** @brief say whether we are going to do random temporal access on this clip, defaults to false */
void ClipDescriptor::setTemporalClipAccess( bool v )
{
	_clipProps.propSetInt( kOfxImageEffectPropTemporalClipAccess, int(v) );
}

/** @brief say whether if the clip is optional, defaults to false */
void ClipDescriptor::setOptional( bool v )
{
	_clipProps.propSetInt( kOfxImageClipPropOptional, int(v) );
}

/** @brief say whether this clip supports tiling, defaults to true */
void ClipDescriptor::setSupportsTiles( bool v )
{
	_clipProps.propSetInt( kOfxImageEffectPropSupportsTiles, int(v) );
}

/** @brief say whether this clip is a 'mask', so the host can know to replace with a roto or similar, defaults to false */
void ClipDescriptor::setIsMask( bool v )
{
	_clipProps.propSetInt( kOfxImageClipPropIsMask, int(v) );
}

////////////////////////////////////////////////////////////////////////////////
// image effect descriptor

/** @brief effect descriptor ctor */
ImageEffectDescriptor::ImageEffectDescriptor( OfxImageEffectHandle handle )
	: _effectHandle( handle )
{
	// fetch the property set handle of the effect
	OfxPropertySetHandle props;
	OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet( handle, &props );

	throwSuiteStatusException( stat );
	_effectProps.propSetHandle( props );

	OFX::Validation::validatePluginDescriptorProperties( props );

	// fetch the param set handle and set it in our ParamSetDescriptor base
	OfxParamSetHandle paramSetHandle;
	stat = OFX::Private::gEffectSuite->getParamSet( handle, &paramSetHandle );
	throwSuiteStatusException( stat );
	setOfxParamSetHandle( paramSetHandle );
}

/** @brief dtor */
ImageEffectDescriptor::~ImageEffectDescriptor()
{
	// delete any clip descriptors we may have constructed
	std::map<std::string, ClipDescriptor*>::iterator iter;
	for( iter = _definedClips.begin(); iter != _definedClips.end(); ++iter )
	{
	if( iter->second )
	{
		delete iter->second;
		iter->second = NULL;
	}
	}
}

/** @brief, set the label properties in a plugin */
void ImageEffectDescriptor::setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel )
{
	_effectProps.propSetString( kOfxPropLabel, label );
	_effectProps.propSetString( kOfxPropShortLabel, shortLabel, false );
	_effectProps.propSetString( kOfxPropLongLabel, longLabel, false );
}

void ImageEffectDescriptor::setDescription( const std::string& description )
{
	// added in openfx API 1.2, so do not throw if unknown by the host
	_effectProps.propSetString( kOfxPropPluginDescription, description, false );
}

/** @brief Set the plugin grouping */
void ImageEffectDescriptor::setPluginGrouping( const std::string& group )
{
	_effectProps.propSetString( kOfxImageEffectPluginPropGrouping, group );
}

/** @brief Add a context to those supported */
void ImageEffectDescriptor::addSupportedContext( EContext v )
{
	int n = _effectProps.propGetDimension( kOfxImageEffectPropSupportedContexts );

	switch( v )
	{
	case eContextGenerator:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGenerator, n );
		break;
	case eContextFilter:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextFilter, n );
		break;
	case eContextTransition:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextTransition, n );
		break;
	case eContextPaint:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextPaint, n );
		break;
	case eContextGeneral:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGeneral, n );
		break;
	case eContextRetimer:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextRetimer, n );
		break;
	case eContextReader:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextReader, n );
		break;
	case eContextWriter:
		_effectProps.propSetString( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextWriter, n );
		break;
	case eContextNone:
		break;
	}
}

void ImageEffectDescriptor::setOverlayInteractDescriptor(EffectInteractWrap* desc)
{
	_overlayDescriptor.reset( desc );
	if( OFX::Private::gHostDescription.supportsOverlays && desc->getMainEntry() )
	_effectProps.propSetPointer( kOfxImageEffectPluginPropOverlayInteractV1, (void*)desc->getMainEntry() );
}

/** @brief Add a pixel depth to those supported */
void ImageEffectDescriptor::addSupportedBitDepth( EBitDepth v )
{
	int n = _effectProps.propGetDimension( kOfxImageEffectPropSupportedPixelDepths );

	switch( v )
	{
	case eBitDepthUByte:
		_effectProps.propSetString( kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthByte, n );
		break;
	case eBitDepthUShort:
		_effectProps.propSetString( kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthShort, n );
		break;
	case eBitDepthFloat:
		_effectProps.propSetString( kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthFloat, n );
		break;
	case eBitDepthNone:
		_effectProps.propSetString( kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthNone, n );
		break;
	case eBitDepthCustom:
		// what the good way ?
		break;
	}
}

/** @brief Add a file extension to those supported */
void ImageEffectDescriptor::addSupportedExtension( const std::string& extension )
{
	// only Tuttle support this property ( out of standard )
	if( OFX::Private::gHostDescription.hostName == "TuttleOfx" )
	{
		const int n = _effectProps.propGetDimension( kTuttleOfxImageEffectPropSupportedExtensions );
		_effectProps.propSetString( kTuttleOfxImageEffectPropSupportedExtensions, extension, n );
	}
}

void ImageEffectDescriptor::addSupportedExtensions( const std::vector<std::string>& extensions )
{
	// only Tuttle support this property ( out of standard )
	if( OFX::Private::gHostDescription.hostName == "TuttleOfx" )
	{
		int n = _effectProps.propGetDimension( kTuttleOfxImageEffectPropSupportedExtensions );

		BOOST_FOREACH( const std::string& ext, extensions )
		{
			_effectProps.propSetString( kTuttleOfxImageEffectPropSupportedExtensions, ext, n++ );
		}
	}
}

void ImageEffectDescriptor::setPluginEvaluation( double evaluation )
{
	// This property is an extension, so it's optional.
	_effectProps.propSetDouble( kTuttleOfxImageEffectPropEvaluation, evaluation, false );
}

/** @brief Is the plugin single instance only ? */
void ImageEffectDescriptor::setSingleInstance( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPluginPropSingleInstance, int(v) );
}

/** @brief Does the plugin expect the host to perform per frame SMP threading */
void ImageEffectDescriptor::setHostFrameThreading( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPluginPropHostFrameThreading, int(v) );
}

/** @brief Does the plugin support multi resolution images */
void ImageEffectDescriptor::setSupportsMultiResolution( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPropSupportsMultiResolution, int(v) );
}

/** @brief Does the plugin support image tiling */
void ImageEffectDescriptor::setSupportsTiles( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPropSupportsTiles, int(v) );
}

/** @brief Does the plugin perform temporal clip access */
void ImageEffectDescriptor::setTemporalClipAccess( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPropTemporalClipAccess, int(v) );
}

/** @brief Does the plugin want to have render called twice per frame in all circumanstances for fielded images ? */
void ImageEffectDescriptor::setRenderTwiceAlways( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPluginPropFieldRenderTwiceAlways, int(v) );
}

/** @brief Does the plugin support inputs and output clips of differing depths */
void ImageEffectDescriptor::setSupportsMultipleClipDepths( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPropSupportsMultipleClipDepths, int(v) );
}

/** @brief Does the plugin support inputs and output clips of pixel aspect ratios */
void ImageEffectDescriptor::setSupportsMultipleClipPARs( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectPropSupportsMultipleClipPARs, int(v) );
}

/** @brief What kind of thread safety does the plugin have */
void ImageEffectDescriptor::setRenderThreadSafety( ERenderSafety v )
{
	switch( v )
	{
	case eRenderUnsafe:
		_effectProps.propSetString( kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderUnsafe );
		break;
	case eRenderInstanceSafe:
		_effectProps.propSetString( kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderInstanceSafe );
		break;
	case eRenderFullySafe:
		_effectProps.propSetString( kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderFullySafe );
		break;
	}
}

/** @brief If the slave param changes the clip preferences need to be re-evaluated */
void ImageEffectDescriptor::addClipPreferencesSlaveParam( ParamDescriptor& p )
{
	int n = _effectProps.propGetDimension( kOfxImageEffectPropClipPreferencesSlaveParam );

	_effectProps.propSetString( kOfxImageEffectPropClipPreferencesSlaveParam, p.getName(), n );
}

/** @brief Create a clip, only callable from describe in context */
ClipDescriptor* ImageEffectDescriptor::defineClip( const std::string& name )
{
	// do we have the clip already
	std::map<std::string, ClipDescriptor*>::const_iterator search;
	search = _definedClips.find( name );
	if( search != _definedClips.end() )
	return search->second;

	// no, so make it
	OfxPropertySetHandle propSet;
	OfxStatus stat = OFX::Private::gEffectSuite->clipDefine( _effectHandle, name.c_str(), &propSet );
	if( stat == kOfxStatOK )
	{
	ClipDescriptor* clip = new ClipDescriptor( name, propSet );

	_definedClips[name]            = clip;
	_clipComponentsPropNames[name] = std::string( "OfxImageClipPropComponents_" ) + name;
	_clipDepthPropNames[name]      = std::string( "OfxImageClipPropDepth_" ) + name;
	_clipPARPropNames[name]        = std::string( "OfxImageClipPropPAR_" ) + name;
	_clipROIPropNames[name]        = std::string( "OfxImageClipPropRoI_" ) + name;
	_clipFrameRangePropNames[name] = std::string( "OfxImageClipPropFrameRange_" ) + name;

	return clip;
	}
	else
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// wraps up an image
Image::Image( OfxPropertySetHandle props )
	: _imageProps( props )
{
	OFX::Validation::validateImageProperties( props );

	// and fetch all the properties
	_pixelData = _imageProps.propGetPointer( kOfxImagePropData );

	_rowDistanceBytes = _imageProps.propGetInt( kOfxImagePropRowBytes );
	_pixelAspectRatio = _imageProps.propGetDouble( kOfxImagePropPixelAspectRatio );

	std::string str = _imageProps.propGetString( kOfxImageEffectPropComponents );
	_pixelComponents = mapPixelComponentStringToEnum( str );

	str         = _imageProps.propGetString( kOfxImageEffectPropPixelDepth );
	_pixelDepth = mapBitDepthStringToEnum( str );

	// compute bytes per pixel
	_pixelBytes = 0;
	switch( _pixelComponents )
	{
	case ePixelComponentRGBA: _pixelBytes   = 4; break;
	case ePixelComponentRGB: _pixelBytes   = 3; break;
	case ePixelComponentAlpha: _pixelBytes  = 1; break;
	case ePixelComponentCustom: _pixelBytes = 0; break;
	case ePixelComponentNone: _pixelBytes   = 0; break;
	}

	switch( _pixelDepth )
	{
	case eBitDepthUByte: _pixelBytes  *= 1; break;
	case eBitDepthUShort: _pixelBytes *= 2; break;
	case eBitDepthFloat: _pixelBytes  *= 4; break;
	case eBitDepthCustom: /* what the good way ? */ break;
	case eBitDepthNone: _pixelBytes = 0; break;
	}

	str                = _imageProps.propGetString( kOfxImageEffectPropPreMultiplication );
	_preMultiplication =  mapPreMultiplicationStringToEnum( str );

	_regionOfDefinition.x1 = _imageProps.propGetInt( kOfxImagePropRegionOfDefinition, 0 );
	_regionOfDefinition.y1 = _imageProps.propGetInt( kOfxImagePropRegionOfDefinition, 1 );
	_regionOfDefinition.x2 = _imageProps.propGetInt( kOfxImagePropRegionOfDefinition, 2 );
	_regionOfDefinition.y2 = _imageProps.propGetInt( kOfxImagePropRegionOfDefinition, 3 );

	_bounds.x1 = _imageProps.propGetInt( kOfxImagePropBounds, 0 );
	_bounds.y1 = _imageProps.propGetInt( kOfxImagePropBounds, 1 );
	_bounds.x2 = _imageProps.propGetInt( kOfxImagePropBounds, 2 );
	_bounds.y2 = _imageProps.propGetInt( kOfxImagePropBounds, 3 );

	str = _imageProps.propGetString( kOfxImagePropField );
	if( str == kOfxImageFieldNone )
	{
	_field = eFieldNone;
	}
	else if( str == kOfxImageFieldBoth )
	{
	_field = eFieldBoth;
	}
	else if( str == kOfxImageFieldLower )
	{
	_field = eFieldLower;
	}
	else if( str == kOfxImageFieldUpper )
	{
	_field = eFieldLower;
	}
	else
	{
	OFX::Log::error( true, "Unknown field state '%s' reported on an image", str.c_str() );
	_field = eFieldNone;
	}

	_uniqueID = _imageProps.propGetString( kOfxImagePropUniqueIdentifier );

	//	std::string tuttleFullName = _imageProps.propGetString( "TuttleFullName" );
	//	TUTTLE_LOG_TRACE("tuttleFullName: " << tuttleFullName );

	_renderScale.x = _imageProps.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	_renderScale.y = _imageProps.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	//std::cout << "IMAGE + " << _uniqueID << std::endl;
}

Image::~Image()
{
	//std::cout << "IMAGE - " << _uniqueID << std::endl;
	OFX::Private::gEffectSuite->clipReleaseImage( _imageProps.propSetHandle() );
}

std::size_t Image::getPixelBytes() const
{
	std::size_t pixelBytes = 0;

	switch( getPixelDepth() )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			return 0; // Unknown
		case OFX::eBitDepthUByte: pixelBytes = 1;
			break;
		case OFX::eBitDepthUShort: pixelBytes = 2;
			break;
		case OFX::eBitDepthFloat: pixelBytes = 4;
			break;
	}

	switch( getPixelComponents() )
	{
		case OFX::ePixelComponentAlpha: break; // pixelSize * 1
		case OFX::ePixelComponentRGB: pixelBytes *= 3;
			break;
		case OFX::ePixelComponentRGBA: pixelBytes *= 4;
			break;
		default: break;
	}
	return pixelBytes;
}

std::size_t Image::getBoundsRowDataBytes() const
{
	return getPixelBytes() * getBoundsSize().x;
}

std::size_t Image::getBoundsNbPixels() const
{
	const OfxPointI s = getBoundsSize();
	return s.x * s.y;
}

std::size_t Image::getBoundsImageDataBytes() const
{
	return getPixelBytes() * getBoundsNbPixels();
}

/** @brief return a pixel pointer
*
* No attempt made to be uber efficient here.
*/
void* Image::getPixelAddress( int x, int y )
{
	// are we in the image bounds
	BOOST_ASSERT( x >= _bounds.x1 && x < _bounds.x2 && y >= _bounds.y1 && y < _bounds.y2 && _pixelBytes != 0 );

	char* pix = ( char* )( ( (char*) _pixelData ) + ( y - _bounds.y1 ) * _rowDistanceBytes );
	pix += ( x - _bounds.x1 ) * _pixelBytes;
	return (void*) pix;
}

////////////////////////////////////////////////////////////////////////////////
// clip instance

/** @brief hidden constructor */
Clip::Clip( ImageEffect* effect, const std::string& name, OfxImageClipHandle handle, OfxPropertySetHandle props )
	: _clipName( name ),
	_clipProps( props ),
	_clipHandle( handle ),
	_effect( effect )
{
	OFX::Validation::validateClipInstanceProperties( _clipProps );
}

/** @brief fetch the labels */
void Clip::getLabels( std::string& label, std::string& shortLabel, std::string& longLabel ) const
{
	label      = _clipProps.propGetString( kOfxPropLabel );
	shortLabel = _clipProps.propGetString( kOfxPropShortLabel, false );
	longLabel  = _clipProps.propGetString( kOfxPropLongLabel, false );
}

/** @brief get the pixel depth */
EBitDepth Clip::getPixelDepth( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageEffectPropPixelDepth );
	EBitDepth bitDepth;

	try
	{
	bitDepth = mapBitDepthStringToEnum( str );
	if( bitDepth == eBitDepthNone && isConnected() )
	{
		OFX::Log::error( true, "Clip %s is connected and has no pixel depth.", _clipName.c_str() );
	}
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str() );
	bitDepth = eBitDepthNone;
	}
	return bitDepth;
}

/** @brief get the components in the image */
EPixelComponent Clip::getPixelComponents( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageEffectPropComponents );
	EPixelComponent pix;

	try
	{
	pix = mapPixelComponentStringToEnum( str );
	if( pix == ePixelComponentNone && isConnected() )
	{
		OFX::Log::error( true, "Clip %s is connected and has no pixel component type!", _clipName.c_str() );
	}
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown  pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str() );
	pix = ePixelComponentNone;
	}
	return pix;
}

/** @brief what is the actual pixel depth of the clip */
EBitDepth Clip::getUnmappedPixelDepth( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageClipPropUnmappedPixelDepth );
	EBitDepth bitDepth;

	try
	{
	bitDepth = mapBitDepthStringToEnum( str );
	if( bitDepth == eBitDepthNone && !isConnected() )
	{
		OFX::Log::error( true, "Clip %s is connected and has no unmapped pixel depth.", _clipName.c_str() );
	}
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown unmapped pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str() );
	bitDepth = eBitDepthNone;
	}
	return bitDepth;
}

/** @brief what is the component type of the clip */
EPixelComponent Clip::getUnmappedPixelComponents( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageClipPropUnmappedComponents );
	EPixelComponent pix;

	try
	{
	pix = mapPixelComponentStringToEnum( str );
	if( pix == ePixelComponentNone && !isConnected() )
	{
		OFX::Log::error( true, "Clip %s is connected and has no unmapped pixel component type!", _clipName.c_str() );
	}
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown unmapped pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str() );
	pix = ePixelComponentNone;
	}
	return pix;
}

/** @brief get the components in the image */
EPreMultiplication Clip::getPreMultiplication( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageEffectPropPreMultiplication );
	EPreMultiplication premult;

	try
	{
	premult = mapPreMultiplicationStringToEnum( str );
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown premultiplication type '%s' reported on clip %s!", str.c_str(), _clipName.c_str() );
	premult = eImageOpaque;
	}
	return premult;
}

/** @brief which spatial field comes first temporally */
EField Clip::getFieldOrder( void ) const
{
	std::string str = _clipProps.propGetString( kOfxImageClipPropFieldOrder );
	EField field;

	try
	{
	field = mapFieldStringToEnum( str );
	OFX::Log::error( field != eFieldNone && field != eFieldLower && field != eFieldUpper,
			"Field order '%s' reported on a clip %s is invalid, it must be none, lower or upper.", str.c_str(), _clipName.c_str() );
	}
	// gone wrong ?
	catch( std::invalid_argument& )
	{
	OFX::Log::error( true, "Unknown field order '%s' reported on a clip %s.", str.c_str(), _clipName.c_str() );
	field = eFieldNone;
	}
	return field;
}

/** @brief is the clip connected */
bool Clip::isConnected( void ) const
{
	return _clipProps.propGetInt( kOfxImageClipPropConnected ) != 0;
}

/** @brief can the clip be continuously sampled */
bool Clip::hasContinuousSamples( void ) const
{
	return _clipProps.propGetInt( kOfxImageClipPropContinuousSamples ) != 0;
}

/** @brief get the scale factor that has been applied to this clip */
double Clip::getPixelAspectRatio( void ) const
{
    try {
	    return _clipProps.propGetDouble( kOfxImagePropPixelAspectRatio );
    }
    catch(...)
    {
        return 1.0;  // This error could happen in Eyeon Fusion.
    }
}

/** @brief get the frame rate, in frames per second on this clip, after any clip preferences have been applied */
double Clip::getFrameRate( void ) const
{
	return _clipProps.propGetDouble( kOfxImageEffectPropFrameRate );
}

/** @brief return the range of frames over which this clip has images, after any clip preferences have been applied */
OfxRangeD Clip::getFrameRange( void ) const
{
	OfxRangeD v;

	v.min = _clipProps.propGetDouble( kOfxImageEffectPropFrameRange, 0 );
	v.max = _clipProps.propGetDouble( kOfxImageEffectPropFrameRange, 1 );
	return v;
}

/** @brief get the frame rate, in frames per second on this clip, before any clip preferences have been applied */
double Clip::getUnmappedFrameRate( void ) const
{
	return _clipProps.propGetDouble( kOfxImageEffectPropUnmappedFrameRate );
}

/** @brief return the range of frames over which this clip has images, before any clip preferences have been applied */
OfxRangeD Clip::getUnmappedFrameRange( void ) const
{
	OfxRangeD v;

	v.min = _clipProps.propGetDouble( kOfxImageEffectPropUnmappedFrameRange, 0 );
	v.max = _clipProps.propGetDouble( kOfxImageEffectPropUnmappedFrameRange, 1 );
	return v;
}

/** @brief get the RoD for this clip in the cannonical coordinate system */
OfxRectD Clip::getCanonicalRod( const OfxTime t ) const
{
	OfxRectD bounds;
	OfxStatus stat = OFX::Private::gEffectSuite->clipGetRegionOfDefinition( _clipHandle, t, &bounds );

	if( stat == kOfxStatFailed )
	{
	bounds.x1 = bounds.x2 = bounds.y1 = bounds.y2 = 0;
	}
	throwSuiteStatusException( stat );
	return bounds;
}

OfxRectD Clip::getCanonicalRod( const OfxTime t, const OfxPointD& renderScale ) const
{
	OfxRectD rod = getCanonicalRod( t );
	rod.x1 *= renderScale.x;
	rod.y1 *= renderScale.y;
	rod.x2 *= renderScale.x;
	rod.y2 *= renderScale.y;
	return rod;
}

/** @brief get the RoD for this clip in pixel space */
OfxRectI Clip::getPixelRod( const OfxTime t ) const
{
	OfxRectD rod = getCanonicalRod(t);
	double ratio = getPixelAspectRatio();
	if( ratio == 0.0 )
		ratio = 1.0;

	OfxRectI pixRod;
	pixRod.x1 = boost::numeric_cast<int>(rod.x1 / ratio);
	pixRod.y1 = boost::numeric_cast<int>(rod.y1);
	pixRod.x2 = boost::numeric_cast<int>(std::ceil(rod.x2 / ratio));
	pixRod.y2 = boost::numeric_cast<int>(std::ceil(rod.y2));

	return pixRod;
}

OfxRectI Clip::getPixelRod( const OfxTime t, const OfxPointD& renderScale ) const
{
	OfxRectI rod = getPixelRod( t );
	rod.x1 = static_cast<int>( rod.x1 * renderScale.x );
	rod.y1 = static_cast<int>( rod.y1 * renderScale.y );
	rod.x2 = static_cast<int>( rod.x2 * renderScale.x );
	rod.y2 = static_cast<int>( rod.y2 * renderScale.y );
	return rod;
}

/** @brief fetch an image */
Image* Clip::fetchImage( OfxTime t )
{
	OfxPropertySetHandle imageHandle;
	OfxStatus stat = OFX::Private::gEffectSuite->clipGetImage( _clipHandle, t, NULL, &imageHandle );

	if( stat == kOfxStatFailed )
	{
	return NULL; // not an error, fetched images out of range/region, assume black and transparent
	}
	else
	throwSuiteStatusException( stat );

	return new Image( imageHandle );
}

/** @brief fetch an image, with a specific region in cannonical coordinates */
Image* Clip::fetchImage( OfxTime t, OfxRectD bounds )
{
	OfxPropertySetHandle imageHandle;
	OfxStatus stat = OFX::Private::gEffectSuite->clipGetImage( _clipHandle, t, &bounds, &imageHandle );

	if( stat == kOfxStatFailed )
	{
	return NULL; // not an error, fetched images out of range/region, assume black and transparent
	}
	else
	throwSuiteStatusException( stat );

	return new Image( imageHandle );
}

////////////////////////////////////////////////////////////////////////////////
/// image effect

/** @brief ctor */
ImageEffect::ImageEffect( OfxImageEffectHandle handle )
	: _effectHandle( handle ),
	_effectProps( 0 ),
	_context( eContextNone ),
	_progressStartSuccess( false )
{
	// get the property handle
	_effectProps = OFX::Private::fetchEffectProps( handle );

	// Set this as the instance data pointer on the effect handle
	_effectProps.propSetPointer( kOfxPropInstanceData, this );

	// validate the plugin instance
	OFX::Validation::validatePluginInstanceProperties( _effectProps );

	// fetch the context
	std::string ctxt = _effectProps.propGetString( kOfxImageEffectPropContext );
	_context = mapContextStringToEnum( ctxt );

	// the param set daddy-oh
	OfxParamSetHandle paramSet;
	OfxStatus stat = OFX::Private::gEffectSuite->getParamSet( handle, &paramSet );
	throwSuiteStatusException( stat );
	setParamSetHandle( paramSet );

}

/** @brief dtor */
ImageEffect::~ImageEffect()
{
	// clobber the instance data property on the effect handle
	_effectProps.propSetPointer( kOfxPropInstanceData, 0 );

	// delete any clip instances we may have constructed
	std::map<std::string, Clip*>::iterator iter;
	for( iter = _fetchedClips.begin(); iter != _fetchedClips.end(); ++iter )
	{
	if( iter->second )
	{
		delete iter->second;
		iter->second = NULL;
	}
	}
}

/// name of the imageEffect
const std::string& ImageEffect::getName() const
{
	return getPropertySet().propGetString( kOfxPropName );
}

/** @brief the context this effect was instantiate in */
EContext ImageEffect::getContext( void ) const
{
	//std::cout << "debug... getContext enum:" << (int)_context << " str:" << mapContextEnumToString(_context) << std::endl;
	return _context;
}

/** @brief size of the project */
OfxPointD ImageEffect::getProjectSize( void ) const
{
	OfxPointD v;

	v.x = _effectProps.propGetDouble( kOfxImageEffectPropProjectSize, 0 );
	v.y = _effectProps.propGetDouble( kOfxImageEffectPropProjectSize, 1 );
	return v;
}

/** @brief origin of the project */
OfxPointD ImageEffect::getProjectOffset( void ) const
{
	OfxPointD v;

	v.x = _effectProps.propGetDouble( kOfxImageEffectPropProjectOffset, 0 );
	v.y = _effectProps.propGetDouble( kOfxImageEffectPropProjectOffset, 1 );
	return v;
}

/** @brief extent of the project */
OfxPointD ImageEffect::getProjectExtent( void ) const
{
	OfxPointD v;

	v.x = _effectProps.propGetDouble( kOfxImageEffectPropProjectExtent, 0 );
	v.y = _effectProps.propGetDouble( kOfxImageEffectPropProjectExtent, 1 );
	return v;
}

/** @brief pixel aspect ratio of the project */
double ImageEffect::getProjectPixelAspectRatio( void ) const
{
	return _effectProps.propGetDouble( kOfxImageEffectPropProjectPixelAspectRatio, 0 );
}

/** @brief how long does the effect last */
double ImageEffect::getEffectDuration( void ) const
{
	return _effectProps.propGetDouble( kOfxImageEffectInstancePropEffectDuration, 0 );
}

/** @brief the frame rate of the project */
double ImageEffect::getFrameRate( void ) const
{
	return _effectProps.propGetDouble( kOfxImageEffectPropFrameRate, 0 );
}

/** @brief is the instance currently being interacted with */
bool ImageEffect::isInteractive( void ) const
{
	return _effectProps.propGetInt( kOfxPropIsInteractive ) != 0;
}

/** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
void ImageEffect::setSequentialRender( bool v )
{
	_effectProps.propSetInt( kOfxImageEffectInstancePropSequentialRender, int(v) );
}

/** @brief Have we informed the host we want to be seqentially renderred ? */
bool ImageEffect::getSequentialRender( void ) const
{
	return _effectProps.propGetInt( kOfxImageEffectInstancePropSequentialRender ) != 0;
}

OFX::Message::EMessageReply ImageEffect::sendMessage( OFX::Message::EMessageType type, const std::string& id, const std::string& msg )
{
	if( !OFX::Private::gMessageSuite->message )
	{
	throwHostMissingSuiteException( "message" );
	}
	OfxStatus stat = OFX::Private::gMessageSuite->message( _effectHandle, mapMessageTypeEnumToString( type ).c_str(), id.c_str(), msg.c_str() );
	return mapMessageReplyStatusToEnum( stat );
}

/** @brief Fetch the named clip from this instance */
Clip* ImageEffect::fetchClip( const std::string& name )
{
	// do we have the clip already
	std::map<std::string, Clip*>::const_iterator search;
	search = _fetchedClips.find( name );
	if( search != _fetchedClips.end() )
	return search->second;

	// fetch the property set handle of the effect
	OfxImageClipHandle clipHandle   = 0;
	OfxPropertySetHandle propHandle = 0;
	OfxStatus stat                  = OFX::Private::gEffectSuite->clipGetHandle( getHandle(), name.c_str(), &clipHandle, &propHandle );
	throwSuiteStatusException( stat );

	// and make one
	Clip* newClip = new Clip( this, name, clipHandle, propHandle );

	// add it in
	_fetchedClips[name] = newClip;

	// return it
	return newClip;
}

/** @brief Fetch a parametric param */
CameraParam* ImageEffect::fetchCameraParam( const std::string& name )
{
	return fetchAttribute<CameraParam>( getHandle(), name );
}

/** @brief does the host want us to abort rendering? */
bool ImageEffect::abort() const
{
	return OFX::Private::gEffectSuite->abort( _effectHandle ) != 0;
}

/** @brief adds a new interact to the set of interacts open on this effect */
void ImageEffect::addOverlayInteract( OverlayInteract* interact )
{
	// do we have it already ?
	std::list<OverlayInteract*>::iterator i;
	i = std::find( _overlayInteracts.begin(), _overlayInteracts.end(), interact );

	// we don't, put it in there
	if( i == _overlayInteracts.end() )
	{
	// we have a new one to add in here
	_overlayInteracts.push_back( interact );
	}
}

/** @brief removes an interact to the set of interacts open on this effect */
void ImageEffect::removeOverlayInteract( OverlayInteract* interact )
{
	// find it
	std::list<OverlayInteract*>::iterator i;
	i = std::find( _overlayInteracts.begin(), _overlayInteracts.end(), interact );

	// and remove it
	if( i != _overlayInteracts.end() )
	{
	_overlayInteracts.erase( i );
	}
}

/** @brief force all overlays on this interact to be redrawn */
void ImageEffect::redrawOverlays( void )
{
	// find it
	std::list<OverlayInteract*>::iterator i;
	for( i = _overlayInteracts.begin(); i != _overlayInteracts.end(); ++i )
	{
	( *i )->requestRedraw();
	}
}

////////////////////////////////////////////////////////////////////////////////
// below are the default members for the base image effect

/** @brief client is identity function, returns the clip and time for the identity function
*/
bool ImageEffect::isIdentity( const RenderArguments& args, Clip*& identityClip, double& identityTime )
{
	return false; // by default, we are not an identity operation
}

/** @brief The get RoD action */
bool ImageEffect::getRegionOfDefinition( const RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	return false; // by default, we are not setting the RoD
}

/** @brief the get RoI action */
void ImageEffect::getRegionsOfInterest( const RegionsOfInterestArguments& args, RegionOfInterestSetter& rois )
{
	// fa niente
}

/** @brief the get frames needed action */
void ImageEffect::getFramesNeeded( const FramesNeededArguments& args, FramesNeededSetter& frames )
{
	// fa niente
}

/** @brief client begin sequence render function */
void ImageEffect::beginSequenceRender( const BeginSequenceRenderArguments& args )
{
	// fa niente
}

/** @brief client end sequence render function, this is one of the few that must be set */
void ImageEffect::endSequenceRender( const EndSequenceRenderArguments& args )
{
	// fa niente
}

/** @brief The purge caches action, a request for an instance to free up as much memory as possible in low memory situations */
void ImageEffect::purgeCaches( void )
{
	// fa niente
}

/** @brief The sync private data action, called when the effect needs to sync any private data to persistant parameters */
void ImageEffect::syncPrivateData( void )
{
	// fa niente
}

/** @brief get the clip preferences */
void ImageEffect::getClipPreferences( ClipPreferencesSetter& clipPreferences )
{
	// fa niente
}

/** @brief the effect is about to be actively edited by a user, called when the first user interface is opened on an instance */
void ImageEffect::beginEdit( void )
{
	// fa niente
}

/** @brief the effect is no longer being edited by a user, called when the last user interface is closed on an instance */
void ImageEffect::endEdit( void )
{
	// fa niente
}

/** @brief the effect is about to have some values changed */
void ImageEffect::beginChanged( InstanceChangeReason reason )
{}

/** @brief called when a param has just had its value changed */
void ImageEffect::changedParam( const InstanceChangedArgs& args, const std::string& paramName )
{}

/** @brief called when a clip has just been changed in some way (a rewire maybe) */
void ImageEffect::changedClip( const InstanceChangedArgs& args, const std::string& clipName )
{}

/** @brief the effect has just had some values changed */
void ImageEffect::endChanged( InstanceChangeReason reason )
{}

/** @brief get the time domain */
bool ImageEffect::getTimeDomain( OfxRangeD& range )
{
	// by default, do the default
	return false;
}

/// Start doing progress.
void ImageEffect::progressStart( const std::string& message )
{
	if( OFX::Private::gProgressSuite )
	{
	OfxStatus stat = OFX::Private::gProgressSuite->progressStart( (void*) _effectHandle, message.c_str() );
	_progressStartSuccess = ( stat == kOfxStatOK );
	}
}

/// finish yer progress
void ImageEffect::progressEnd()
{
	if( OFX::Private::gProgressSuite && _progressStartSuccess )
	{
	OFX::Private::gProgressSuite->progressEnd( (void*) _effectHandle );
	}
}

/// set the progress to some level of completion,
/// returns true if you should abandon processing, false to continue
bool ImageEffect::progressUpdate( const double t )
{
	if( OFX::Private::gProgressSuite && _progressStartSuccess )
	{
	const OfxStatus stat = OFX::Private::gProgressSuite->progressUpdate( (void*) _effectHandle, t );
	if( stat == kOfxStatReplyNo )
		return true;
	}
	return false;
}

/// get the current time on the timeline. This is not necessarily the same
/// time as being passed to an action (eg render)
double ImageEffect::timeLineGetTime()
{
	if( OFX::Private::gTimeLineSuite )
	{
	double time;
	if( OFX::Private::gTimeLineSuite->getTime( (void*) _effectHandle, &time ) == kOfxStatOK )
		return time;
	}
	return 0;
}

/// set the timeline to a specific time
void ImageEffect::timeLineGotoTime( const double t )
{
	if( OFX::Private::gTimeLineSuite )
	{
	OFX::Private::gTimeLineSuite->gotoTime( (void*) _effectHandle, t );
	}
}

/// get the first and last times available on the effect's timeline
void ImageEffect:: timeLineGetBounds( double& t1, double& t2 )
{
	t1 = t2 = 0;
	if( OFX::Private::gTimeLineSuite )
	{
	OFX::Private::gTimeLineSuite->getTimeBounds( (void*) _effectHandle, &t1, &t2 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// Class used to set the clip preferences of the effect. */

const std::string& ClipPreferencesSetter::extractValueForName( const StringStringMap& m, const std::string& name )
{
	StringStringMap::const_iterator it = m.find( name );

	if( it == m.end() )
	BOOST_THROW_EXCEPTION( Exception::PropertyUnknownToHost( name ) );
	return it->second;
}

/** @brief, force the host to set a clip's mapped component type to be \em comps.  */
void ClipPreferencesSetter::setClipComponents( Clip& clip, EPixelComponent comps )
{
	doneSomething_ = true;
	const std::string& propName = extractValueForName( clipComponentPropNames_, clip.name() );

	switch( comps )
	{
	case ePixelComponentRGBA:
		outArgs_.propSetString( propName.c_str(), kOfxImageComponentRGBA );
		break;
	case ePixelComponentRGB:
		outArgs_.propSetString( propName.c_str(), kOfxImageComponentRGB );
		break;
	case ePixelComponentAlpha:
		outArgs_.propSetString( propName.c_str(), kOfxImageComponentAlpha );
		break;
	case ePixelComponentCustom:
		break;
	case ePixelComponentNone:
		outArgs_.propSetString( propName.c_str(), kOfxImageComponentNone );
		break;
	}
}

/** @brief, force the host to set a clip's mapped bit depth be \em bitDepth */
void ClipPreferencesSetter::setClipBitDepth( Clip& clip, EBitDepth bitDepth )
{
	doneSomething_ = true;
	const std::string& propName = extractValueForName( clipDepthPropNames_, clip.name() );

	if( _imageEffectHostDescription->supportsMultipleClipDepths )
	{
	switch( bitDepth )
	{
		case eBitDepthUByte:
		outArgs_.propSetString( propName.c_str(), kOfxBitDepthByte );
		break;
		case eBitDepthUShort:
		outArgs_.propSetString( propName.c_str(), kOfxBitDepthShort );
		break;
		case eBitDepthFloat:
		outArgs_.propSetString( propName.c_str(), kOfxBitDepthFloat );
		break;
		case eBitDepthNone:
		outArgs_.propSetString( propName.c_str(), kOfxBitDepthNone );
		break;
		case eBitDepthCustom:
		// what the good way ?
		break;
	}
	}
	else
	{
	// BOOST_THROW_EXCEPTION( std::logic_error("Host doesn't support multiple bit depths.") );
	// it's not clear what we need to to in this case...
	// set the value supported by the host or set nothing
	// by setting this value, we may have less problem depending on host implementations
	outArgs_.propSetString( propName.c_str(), mapBitDepthEnumToString( _imageEffectHostDescription->getPixelDepth() ) );
	}
}

/** @brief, force the host to set a clip's mapped Pixel Aspect Ratio to be \em PAR */
void ClipPreferencesSetter::setPixelAspectRatio( Clip& clip, double PAR )
{
	if( ! _imageEffectHostDescription->supportsMultipleClipPARs )
	return;
//		BOOST_THROW_EXCEPTION( std::logic_error("Host doesn't support multiple Pixel Aspect Ratios.") );

	doneSomething_ = true;
	const std::string& propName = extractValueForName( clipPARPropNames_, clip.name() );
	outArgs_.propSetDouble( propName.c_str(), PAR );
}

/** @brief Allows an effect to change the output frame rate */
void ClipPreferencesSetter::setOutputFrameRate( double v )
{
	if( ! _imageEffectHostDescription->supportsSetableFrameRate )
	return;
//		BOOST_THROW_EXCEPTION( std::logic_error("Host doesn't support setable frame rate.") );

	doneSomething_ = true;
	outArgs_.propSetDouble( kOfxImageEffectPropFrameRate, v );
}

/** @brief Set the premultiplication state of the output clip. */
void ClipPreferencesSetter::setOutputPremultiplication( EPreMultiplication v )
{
	doneSomething_ = true;
	switch( v )
	{
	case eImageOpaque:
		outArgs_.propSetString( kOfxImageEffectPropPreMultiplication, kOfxImageOpaque );
		break;
	case eImagePreMultiplied:
		outArgs_.propSetString( kOfxImageEffectPropPreMultiplication, kOfxImagePreMultiplied );
		break;
	case eImageUnPreMultiplied:
		outArgs_.propSetString( kOfxImageEffectPropPreMultiplication, kOfxImageUnPreMultiplied );
		break;
	}
}

/** @brief Set whether the effect can be continously sampled. */
void ClipPreferencesSetter::setOutputHasContinousSamples( bool v )
{
	doneSomething_ = true;
	outArgs_.propSetInt( kOfxImageClipPropContinuousSamples, int(v) );
}

/** @brief Sets whether the effect will produce different images in all frames, even if the no params or input images are varying (eg: a noise generator). */
void ClipPreferencesSetter::setOutputFrameVarying( bool v )
{
	doneSomething_ = true;
	outArgs_.propSetInt( kOfxImageEffectFrameVarying, int(v) );
}

void ClipPreferencesSetter::setOutputFielding( EField v )
{
	doneSomething_ = true;
	switch( v )
	{
	case eFieldNone: outArgs_.propSetString( kOfxImageClipPropFieldOrder, kOfxImageFieldNone, 0, false ); break;
	case eFieldLower: outArgs_.propSetString( kOfxImageClipPropFieldOrder, kOfxImageFieldLower, 0, false ); break;
	case eFieldUpper: outArgs_.propSetString( kOfxImageClipPropFieldOrder, kOfxImageFieldUpper, 0, false ); break;
	case eFieldBoth: outArgs_.propSetString( kOfxImageClipPropFieldOrder, kOfxImageFieldBoth, 0, false ); break;
		// kOfxImageFieldSingle ?
		// kOfxImageFieldDoubled ?
	}
}

////////////////////////////////////////////////////////////////////////////////
/** @brief Class that skins image memory allocation */

ImageMemory::ImageMemory()
	: _handle( 0 )
	, _alloc(false)
{
}

ImageMemory::ImageMemory( size_t nBytes, ImageEffect* associatedEffect )
	: _handle( 0 )
	, _alloc(false)
{
	alloc( nBytes, associatedEffect );
}

/** @brief dtor */
ImageMemory::~ImageMemory()
{
	if( _alloc )
	{
		/*OfxStatus stat = */ OFX::Private::gEffectSuite->imageMemoryFree( _handle );
	}
	// ignore status code for exception purposes
}

void ImageMemory::alloc( size_t nBytes, ImageEffect* associatedEffect )
{
	BOOST_ASSERT( ! _alloc );
	_alloc = true;
	OfxImageEffectHandle effectHandle = 0;

	if( associatedEffect != 0 )
	{
		effectHandle = associatedEffect->_effectHandle;
	}

	OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryAlloc( effectHandle, nBytes, &_handle );
	if( stat == kOfxStatErrMemory )
		BOOST_THROW_EXCEPTION( std::bad_alloc() );
	throwSuiteStatusException( stat );
}

/** @brief lock the memory and return a pointer to it */
void* ImageMemory::lock( void )
{
	void* ptr;
	OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryLock( _handle, &ptr );

	if( stat == kOfxStatErrMemory )
	BOOST_THROW_EXCEPTION( std::bad_alloc() );
	throwSuiteStatusException( stat );
	return ptr;
}

/** @brief unlock the memory */
void ImageMemory::unlock( void )
{
	/*OfxStatus stat = */ OFX::Private::gEffectSuite->imageMemoryUnlock( _handle );
}

/** @brief OFX::Private namespace, for things private to the support library code here generally calls image effect class members */
namespace Private {

/** @brief Creates the global host description and sets its properties */
void fetchHostDescription( OfxHost* host )
{
	OFX::Log::error( OFX::Private::gHostDescriptionHasInit, "Tried to create host description when we already have one." );
	if( !OFX::Private::gHostDescriptionHasInit )
	{
	OFX::Private::gHostDescriptionHasInit = true;
	// wrap the property handle up with a property set
	PropertySet hostProps( host->host );

	// and get some properties
	gHostDescription.hostName                    = hostProps.propGetString( kOfxPropName );
	gHostDescription.hostLabel                   = hostProps.propGetString( kOfxPropLabel );
	gHostDescription.hostIsBackground            = hostProps.propGetInt( kOfxImageEffectHostPropIsBackground ) != 0;
	gHostDescription.supportsOverlays            = hostProps.propGetInt( kOfxImageEffectPropSupportsOverlays ) != 0;
	gHostDescription.supportsMultiResolution     = hostProps.propGetInt( kOfxImageEffectPropSupportsMultiResolution ) != 0;
	gHostDescription.supportsTiles               = hostProps.propGetInt( kOfxImageEffectPropSupportsTiles ) != 0;
	gHostDescription.temporalClipAccess          = hostProps.propGetInt( kOfxImageEffectPropTemporalClipAccess ) != 0;
	gHostDescription.supportsMultipleClipDepths  = hostProps.propGetInt( kOfxImageEffectPropSupportsMultipleClipDepths ) != 0;
	gHostDescription.supportsMultipleClipPARs    = hostProps.propGetInt( kOfxImageEffectPropSupportsMultipleClipPARs ) != 0;
	gHostDescription.supportsSetableFrameRate    = hostProps.propGetInt( kOfxImageEffectPropSetableFrameRate ) != 0;
	gHostDescription.supportsSetableFielding     = hostProps.propGetInt( kOfxImageEffectPropSetableFielding ) != 0;
	gHostDescription.supportsStringAnimation     = hostProps.propGetInt( kOfxParamHostPropSupportsStringAnimation ) != 0;
	gHostDescription.supportsCustomInteract      = hostProps.propGetInt( kOfxParamHostPropSupportsCustomInteract ) != 0;
	gHostDescription.supportsChoiceAnimation     = hostProps.propGetInt( kOfxParamHostPropSupportsChoiceAnimation ) != 0;
	gHostDescription.supportsBooleanAnimation    = hostProps.propGetInt( kOfxParamHostPropSupportsBooleanAnimation ) != 0;
	gHostDescription.supportsCustomAnimation     = hostProps.propGetInt( kOfxParamHostPropSupportsCustomAnimation ) != 0;
	gHostDescription.supportsParametricParameter = gParametricParameterSuite != NULL;
	gHostDescription.supportsCameraParameter     = gCameraParameterSuite != NULL;
	gHostDescription.maxParameters               = hostProps.propGetInt( kOfxParamHostPropMaxParameters );
	gHostDescription.maxPages                    = hostProps.propGetInt( kOfxParamHostPropMaxPages );
	gHostDescription.pageRowCount                = hostProps.propGetInt( kOfxParamHostPropPageRowColumnCount, 0 );
	gHostDescription.pageColumnCount             = hostProps.propGetInt( kOfxParamHostPropPageRowColumnCount, 1 );
	int numComponents = hostProps.propGetDimension( kOfxImageEffectPropSupportedComponents );
	for( int i = 0; i < numComponents; ++i )
		gHostDescription._supportedComponents.push_back( mapPixelComponentStringToEnum( hostProps.propGetString( kOfxImageEffectPropSupportedComponents, i ) ) );

	int numContexts = hostProps.propGetDimension( kOfxImageEffectPropSupportedContexts );
	for( int i = 0; i < numContexts; ++i )
		gHostDescription._supportedContexts.push_back( mapContextStringToEnum( hostProps.propGetString( kOfxImageEffectPropSupportedContexts, i ) ) );

	int numPixelDepths = hostProps.propGetDimension( kOfxImageEffectPropSupportedPixelDepths );
	for( int i = 0; i < numPixelDepths; ++i )
		gHostDescription._supportedPixelDepths.push_back( mapBitDepthStringToEnum( hostProps.propGetString( kOfxImageEffectPropSupportedPixelDepths, i ) ) );
	}
}

/** @brief fetch the effect property set from the ImageEffectHandle */
OFX::PropertySet fetchEffectProps( OfxImageEffectHandle handle )
{
	// get the property handle
	OfxPropertySetHandle propHandle;
	OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet( handle, &propHandle );

	throwSuiteStatusException( stat );
	return OFX::PropertySet( propHandle );
}

/** @brief Library side load action, this fetches all the suite pointers */
void loadAction( void )
{
	++Private::gLoadCount;

	//OfxStatus status = kOfxStatOK;

	// fetch the suites
	OFX::Log::error( gHost == 0, "Host pointer has not been set." );
	if( !gHost )
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrBadHandle ) );

	if( Private::gLoadCount == 1 )
	{
		gEffectSuite   = (OfxImageEffectSuiteV1*) fetchSuite( kOfxImageEffectSuite, 1 );
		gPropSuite     = (OfxPropertySuiteV1*)    fetchSuite( kOfxPropertySuite, 1 );
		gParamSuite    = (OfxParameterSuiteV1*)   fetchSuite( kOfxParameterSuite, 1 );
		gMemorySuite   = (OfxMemorySuiteV1*)      fetchSuite( kOfxMemorySuite, 1 );
		gThreadSuite   = (OfxMultiThreadSuiteV1*) fetchSuite( kOfxMultiThreadSuite, 1 );
		gMessageSuite  = (OfxMessageSuiteV1*)     fetchSuite( kOfxMessageSuite, 1 );
		gProgressSuite = (OfxProgressSuiteV1*)     fetchSuite( kOfxProgressSuite, 1, true );
		gTimeLineSuite = (OfxTimeLineSuiteV1*)     fetchSuite( kOfxTimeLineSuite, 1, true );
		gParametricParameterSuite = static_cast<OfxParametricParameterSuiteV1*>( OFX::fetchSuite( kOfxParametricParameterSuite, 1, true ) );
		gCameraParameterSuite = static_cast<NukeOfxCameraSuiteV1*>( OFX::fetchSuite( kNukeOfxCameraSuite, 1, true ) );

		// OK check and fetch host information
		fetchHostDescription( gHost );

		/// and set some dendent flags
		OFX::Private::gHostDescription.supportsProgressSuite = ( gProgressSuite != NULL );
		OFX::Private::gHostDescription.supportsTimeLineSuite = ( gTimeLineSuite != NULL );

		// fetch the interact suite if the host supports interaction
		if( OFX::Private::gHostDescription.supportsOverlays || OFX::Private::gHostDescription.supportsCustomInteract )
			gInteractSuite = (OfxInteractSuiteV1*) fetchSuite( kOfxInteractSuite, 1 );
	}

	// initialise the validation code
	OFX::Validation::initialise();

	// validate the host
	OFX::Validation::validateHostProperties( gHost );

}

/** @brief Library side unload action, this fetches all the suite pointers */
void unloadAction( const char* id )
{
	--Private::gLoadCount;

    if( Private::gLoadCount < 0 )
    {
        std::cerr << "OFX Plugin \"" << id << "\" is already unloaded." << std::endl;
        return;
    }

	{
		EffectDescriptorMap::iterator it = gEffectDescriptors.find( id );
		EffectContextMap& toBeDeleted    = it->second;
		for( EffectContextMap::iterator it2 = toBeDeleted.begin(); it2 != toBeDeleted.end(); ++it2 )
		{
			OFX::ImageEffectDescriptor* desc = it2->second;
			delete desc;
		}
		toBeDeleted.clear();
	}
	{
		OFX::OfxPlugInfoMap::iterator it  = OFX::Private::plugInfoMap.find( id );
		if( it == OFX::Private::plugInfoMap.end() )
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrBadIndex, std::string("Can't unload the plugin \"") + id + "\".") );
		OfxPlugin* plugin = it->second._plug;
		OFX::OfxPluginArray::iterator it2 = std::find( ofxPlugs.begin(), ofxPlugs.end(), plugin );
		if( it2 == ofxPlugs.end() )
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrBadIndex, std::string("Can't unload the plugin \"") + id + "\".") );
		(*it2) = NULL; // set the plugin as unloaded but keep the index
		delete plugin;
		//it->second._plug = NULL; // delete the plugin but keep an entry into the map
	}

	if( Private::gLoadCount == 0 )
	{
		// force these to null
		gEffectSuite   = NULL;
		gPropSuite     = NULL;
		gParamSuite    = NULL;
		gMemorySuite   = NULL;
		gThreadSuite   = NULL;
		gMessageSuite  = NULL;
		gInteractSuite = NULL;
		gParametricParameterSuite = NULL;
		gCameraParameterSuite = NULL;

		ofxPlugs.clear();
		gHasInit = false;
	}
}

/** @brief fetches our pointer out of the props on the handle */
ImageEffect* retrieveImageEffectPointer( OfxImageEffectHandle handle )
{
	ImageEffect* instance;

	// get the prop set on the handle
	OfxPropertySetHandle propHandle;
	OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet( handle, &propHandle );

	throwSuiteStatusException( stat );

	// make our wrapper object
	PropertySet props( propHandle );

	// fetch the instance data out of the properties
	instance = (ImageEffect*) props.propGetPointer( kOfxPropInstanceData );

	OFX::Log::error( instance == NULL, "Instance data handle in effect instance properties is NULL!" );

	if( instance == NULL )
	{
	BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrBadHandle, std::string("Can't retrieve ImageEffect pointer from ofxImageEffectHandle. (property: ") + kOfxPropInstanceData + " is NULL).\nThe plugin will crash...") );
	}
	// and dance to the music
	return instance;
}

/** @brief Checks the handles passed into the plugin's main entry point */
void checkMainHandles( const std::string& action,  const void* handle,
			OfxPropertySetHandle inArgsHandle,  OfxPropertySetHandle outArgsHandle,
			bool handleCanBeNull, bool inArgsCanBeNull, bool outArgsCanBeNull )
{
	if( handleCanBeNull )
		OFX::Log::warning( handle != 0, "Handle passed to '%s' is not null.", action.c_str() );
	else
		OFX::Log::error( handle == 0, "'Handle passed to '%s' is null.", action.c_str() );

	if( inArgsCanBeNull )
		OFX::Log::warning( inArgsHandle != 0, "'inArgs' Handle passed to '%s' is not null.", action.c_str() );
	else
		OFX::Log::error( inArgsHandle == 0, "'inArgs' handle passed to '%s' is null.", action.c_str() );

	if( outArgsCanBeNull )
		OFX::Log::warning( outArgsHandle != 0, "'outArgs' Handle passed to '%s' is not null.", action.c_str() );
	else
		OFX::Log::error( outArgsHandle == 0, "'outArgs' handle passed to '%s' is null.", action.c_str() );

	// validate the property sets on the arguments
	OFX::Validation::validateActionArgumentsProperties( action, inArgsHandle, outArgsHandle );

	// throw exceptions if null when not meant to be null
	if( !handleCanBeNull && !handle )
		throwSuiteStatusException( kOfxStatErrBadHandle );
	if( !inArgsCanBeNull && !inArgsHandle )
		throwSuiteStatusException( kOfxStatErrBadHandle );
	if( !outArgsCanBeNull && !outArgsHandle )
		throwSuiteStatusException( kOfxStatErrBadHandle );
}

/** @brief Fetches the arguments used in a render action 'inargs' property set into a POD struct */
void getRenderActionArguments( RenderArguments& args,  OFX::PropertySet inArgs )
{
	args.time = inArgs.propGetDouble( kOfxPropTime );

	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	args.renderWindow.x1 = inArgs.propGetInt( kOfxImageEffectPropRenderWindow, 0 );
	args.renderWindow.y1 = inArgs.propGetInt( kOfxImageEffectPropRenderWindow, 1 );
	args.renderWindow.x2 = inArgs.propGetInt( kOfxImageEffectPropRenderWindow, 2 );
	args.renderWindow.y2 = inArgs.propGetInt( kOfxImageEffectPropRenderWindow, 3 );

	std::string str = inArgs.propGetString( kOfxImageEffectPropFieldToRender );
	try
	{
		args.fieldToRender = mapFieldStringToEnum( str );
	}
	catch( std::invalid_argument )
	{
		// dud field?
		OFX::Log::error( true, "Unknown field to render '%s'", str.c_str() );

		// HACK need to throw something to cause a failure
	}
}

/** @brief Library side render action, fetches relevant properties and calls the client code */
void renderAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );
	RenderArguments args;

	// get the arguments
	getRenderActionArguments( args, inArgs );

	// and call the plugin client render code
	effectInstance->render( args );
}

/** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
void beginSequenceRenderAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	BeginSequenceRenderArguments args;

	args.frameRange.min = inArgs.propGetDouble( kOfxImageEffectPropFrameRange, 0 );
	args.frameRange.max = inArgs.propGetDouble( kOfxImageEffectPropFrameRange, 1 );

	args.frameStep = inArgs.propGetDouble( kOfxImageEffectPropFrameStep, 0 );

	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	args.isInteractive = inArgs.propGetInt( kOfxPropIsInteractive ) != 0;

	// and call the plugin client render code
	effectInstance->beginSequenceRender( args );
}

/** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
void endSequenceRenderAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	EndSequenceRenderArguments args;

	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	args.isInteractive = inArgs.propGetInt( kOfxPropIsInteractive ) != 0;

	// and call the plugin client render code
	effectInstance->endSequenceRender( args );
}

/** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
bool isIdentityAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet& outArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );
	RenderArguments args;

	// get the arguments
	getRenderActionArguments( args, inArgs );

	// and call the plugin client isIdentity code
	Clip* identityClip  = 0;
	double identityTime = args.time;
	bool v              = effectInstance->isIdentity( args, identityClip, identityTime );

	if( v )
	{
		// This is not possible to put an empty clip in the OpenFX standard, but it's allowed in TuttleOFX.
		outArgs.propSetString( kOfxPropName, identityClip ? identityClip->name() : "" );
		outArgs.propSetDouble( kOfxPropTime, identityTime );
		return true;
	}
	return false;
}

/** @brief Library side get region of definition function */
bool regionOfDefinitionAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet& outArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );
	RegionOfDefinitionArguments args;

	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	args.time = inArgs.propGetDouble( kOfxPropTime );

	// and call the plugin client code
	OfxRectD rod;
	rod.x1 = outArgs.propGetDouble(kOfxImageEffectPropRegionOfDefinition, 0);
	rod.y1 = outArgs.propGetDouble(kOfxImageEffectPropRegionOfDefinition, 1);
	rod.x2 = outArgs.propGetDouble(kOfxImageEffectPropRegionOfDefinition, 2);
	rod.y2 = outArgs.propGetDouble(kOfxImageEffectPropRegionOfDefinition, 3);
	bool v = effectInstance->getRegionOfDefinition( args, rod );

	if( v )
	{
	outArgs.propSetDouble( kOfxImageEffectPropRegionOfDefinition, rod.x1, 0 );
	outArgs.propSetDouble( kOfxImageEffectPropRegionOfDefinition, rod.y1, 1 );
	outArgs.propSetDouble( kOfxImageEffectPropRegionOfDefinition, rod.x2, 2 );
	outArgs.propSetDouble( kOfxImageEffectPropRegionOfDefinition, rod.y2, 3 );
	return true;
	}
	return false;
}

/** @brief Library side get regions of interest function */
bool regionsOfInterestAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet& outArgs, const char* plugname )
{
	/** @brief local class to set the roi of a clip */
	class ActualROISetter : public OFX::RegionOfInterestSetter
	{
	bool doneSomething_;
	OFX::PropertySet& outArgs_;
	const std::map<std::string, std::string>& clipROIPropNames_;
	const char* _plugname;

	public:
	/** @brief ctor */
	ActualROISetter( OFX::PropertySet& args, const std::map<std::string, std::string>& clipROIPropNames )
		: doneSomething_( false ),
		outArgs_( args ),
		clipROIPropNames_( clipROIPropNames )
	{}

	/** @brief did we set something ? */
	bool didSomething( void ) const { return doneSomething_; }

	/** @brief set the RoI of the clip */
	virtual void setRegionOfInterest( const Clip& clip, const OfxRectD& roi )
	{
		std::map<std::string, std::string>::const_iterator it = clipROIPropNames_.find( clip.name() );
		if( it == clipROIPropNames_.end() )
		BOOST_THROW_EXCEPTION( Exception::PropertyUnknownToHost( clip.name() ) );

		// construct the name of the property
		const std::string& propName = it->second;

		// and set it
		outArgs_.propSetDouble( propName.c_str(), roi.x1, 0 );
		outArgs_.propSetDouble( propName.c_str(), roi.y1, 1 );
		outArgs_.propSetDouble( propName.c_str(), roi.x2, 2 );
		outArgs_.propSetDouble( propName.c_str(), roi.y2, 3 );

		// and record the face we have done something
		doneSomething_ = true;
	}

	}; // end of local class

	// fetch our effect pointer
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );
	RegionsOfInterestArguments args;

	// fetch in arguments from the prop handle
	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	args.regionOfInterest.x1 = inArgs.propGetDouble( kOfxImageEffectPropRegionOfInterest, 0 );
	args.regionOfInterest.y1 = inArgs.propGetDouble( kOfxImageEffectPropRegionOfInterest, 1 );
	args.regionOfInterest.x2 = inArgs.propGetDouble( kOfxImageEffectPropRegionOfInterest, 2 );
	args.regionOfInterest.y2 = inArgs.propGetDouble( kOfxImageEffectPropRegionOfInterest, 3 );

	args.time = inArgs.propGetDouble( kOfxPropTime );

	// make a roi setter object
	ActualROISetter setRoIs( outArgs, gEffectDescriptors[plugname][effectInstance->getContext()]->getClipROIPropNames() );

	// and call the plugin client code
	effectInstance->getRegionsOfInterest( args, setRoIs );

	// did we do anything ?
	if( setRoIs.didSomething() )
	return true;
	return false;
}

/** @brief Library side frames needed action */
bool framesNeededAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet& outArgs, const char* plugname )
{
	/** @brief local class to set the frames needed from a clip */
	class ActualSetter : public OFX::FramesNeededSetter
	{
	OFX::PropertySet& outArgs_;                                  /**< @brief property set to set values in */
	std::map<std::string, std::vector<OfxRangeD> > frameRanges_;  /**< @brief map holding a bunch of frame ranges, one for each clip */
	const std::map<std::string, std::string>& _clipFrameRangePropNames;

	public:
	/** @brief ctor */
	ActualSetter( OFX::PropertySet& args, const std::map<std::string, std::string>& clipFrameRangePropNames )
		: outArgs_( args ),
		_clipFrameRangePropNames( clipFrameRangePropNames )
	{}

	/** @brief set the RoI of the clip */
	virtual void setFramesNeeded( const Clip& clip, const OfxRangeD& range )
	{
		// insert this into the vector which is in the map
		frameRanges_[clip.name()].push_back( range );
	}

	/** @brief write frameRanges_ back to the property set */
	bool setOutProperties( void )
	{
		bool didSomething = false;

		std::map<std::string, std::vector<OfxRangeD> >::iterator i;

		for( i = frameRanges_.begin(); i != frameRanges_.end(); ++i )
		{
		if( i->first != kOfxImageEffectOutputClipName )
		{
			didSomething = true;

			// Make the property name we are setting
			const std::map<std::string, std::string>::const_iterator it = _clipFrameRangePropNames.find( i->first );
			if( it == _clipFrameRangePropNames.end() )
			BOOST_THROW_EXCEPTION( Exception::PropertyUnknownToHost( i->first ) );

			const std::string& propName = it->second;

			// fetch the list of frame ranges
			std::vector<OfxRangeD>& clipRange = i->second;
			std::vector<OfxRangeD>::iterator j;
			int n = 0;

			// and set 'em
			for( j = clipRange.begin(); j < clipRange.end(); ++j )
			{
			outArgs_.propSetDouble( propName.c_str(), j->min, n++ );
			outArgs_.propSetDouble( propName.c_str(), j->max, n++ );
			}
		}
		}

		return didSomething;
	}

	}; // end of local class

	// fetch our effect pointer
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );
	FramesNeededArguments args;

	// fetch in arguments from the prop handle
	args.time = inArgs.propGetDouble( kOfxPropTime );

	// make a roi setter object
	ActualSetter setFrames( outArgs, gEffectDescriptors[plugname][effectInstance->getContext()]->getClipFrameRangePropNames() );

	// and call the plugin client code
	effectInstance->getFramesNeeded( args, setFrames );

	// Write it back to the properties and see if we set anything
	if( setFrames.setOutProperties() )
	return true;
	return false;
}

/** @brief Library side get regions of interest function */
bool getTimeDomainAction( OfxImageEffectHandle handle, OFX::PropertySet& outArgs )
{
	// fetch our effect pointer
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	// we can only be a general context effect, so check that this is true
	OFX::Log::error(
		effectInstance->getContext() != eContextGeneral &&
		effectInstance->getContext() != eContextReader &&
		effectInstance->getContext() != eContextGenerator
		,
		"Calling kOfxImageEffectActionGetTimeDomain on an effect that is not a 'general', 'reader' or 'generator' context node." );

	OfxRangeD timeDomain;
	timeDomain.min = outArgs.propGetDouble( kOfxImageEffectPropFrameRange, 0 );
	timeDomain.max = outArgs.propGetDouble( kOfxImageEffectPropFrameRange, 1 );

	// and call the plugin client code
	bool v = effectInstance->getTimeDomain( timeDomain );

	if( v )
	{
	outArgs.propSetDouble( kOfxImageEffectPropFrameRange, timeDomain.min, 0 );
	outArgs.propSetDouble( kOfxImageEffectPropFrameRange, timeDomain.max, 1 );
	}

	return v;
}

/** @brief Library side get regions of interest function */
bool clipPreferencesAction( OfxImageEffectHandle handle, OFX::PropertySet& outArgs, const char* plugname )
{
	// fetch our effect pointer
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	// set up our clip preferences setter
	ImageEffectDescriptor* desc = gEffectDescriptors[plugname][effectInstance->getContext()];
	ClipPreferencesSetter prefs( outArgs, desc->getClipDepthPropNames(), desc->getClipComponentPropNames(), desc->getClipPARPropNames() );

	// and call the plug-in client code
	effectInstance->getClipPreferences( prefs );

	// did we do anything ?
	if( prefs.didSomething() )
	return true;
	return false;
}

/** @brief Library side begin instance changed action */
void beginInstanceChangedAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	std::string reasonStr       = inArgs.propGetString( kOfxPropChangeReason );
	InstanceChangeReason reason = mapInstanceChangedReasonStringToEnum( reasonStr );

	// and call the plugin client code
	effectInstance->beginChanged( reason );
}

/** @brief Library side instance changed action */
void instanceChangedAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	InstanceChangedArgs args;

	// why did it change
	std::string reasonStr = inArgs.propGetString( kOfxPropChangeReason );

	args.reason        = mapInstanceChangedReasonStringToEnum( reasonStr );
	args.time          = inArgs.propGetDouble( kOfxPropTime );
	args.renderScale.x = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 0 );
	args.renderScale.y = inArgs.propGetDouble( kOfxImageEffectPropRenderScale, 1 );

	// what changed
	std::string changedType = inArgs.propGetString( kOfxPropType );
	std::string changedName = inArgs.propGetString( kOfxPropName );

	if( changedType == kOfxTypeParameter )
	{
	// and call the plugin client code
	effectInstance->changedParam( args, changedName );
	}
	else if( changedType == kOfxTypeClip )
	{
	// and call the plugin client code
	effectInstance->changedClip( args, changedName );
	}
	else
	{
	OFX::Log::error( true, "Instance Changed called with unknown type '%s' of object '%s'", changedType.c_str(), changedName.c_str() );
	}
}

/** @brief Library side end instance changed action */
void endInstanceChangedAction( OfxImageEffectHandle handle, OFX::PropertySet inArgs )
{
	ImageEffect* effectInstance = retrieveImageEffectPointer( handle );

	std::string reasonStr       = inArgs.propGetString( kOfxPropChangeReason );
	InstanceChangeReason reason = mapInstanceChangedReasonStringToEnum( reasonStr );

	// and call the plugin client code
	effectInstance->endChanged( reason );
}

/** @brief The main entry point for the plugin
*/
OfxStatus mainEntryStr( const char*          actionRaw,
			const void*          handleRaw,
			OfxPropertySetHandle inArgsRaw,
			OfxPropertySetHandle outArgsRaw,
			const char*          plugname )
{
	OFX::Log::print( "********************************************************************************" );
	OFX::Log::print( "START mainEntry (%s)", actionRaw );
	OFX::Log::indent();
	OfxStatus stat = kOfxStatReplyDefault;

	// Cast the raw handle to be an image effect handle, because that is what it is
	OfxImageEffectHandle handle = (OfxImageEffectHandle) handleRaw;

	// Turn the arguments into wrapper objects to make our lives easier
	OFX::PropertySet inArgs( inArgsRaw );
	OFX::PropertySet outArgs( outArgsRaw );

	try
	{
		// turn the action into a std::string
		std::string action( actionRaw );
		OfxPlugInfoMap::iterator it = plugInfoMap.find( plugname );
		if( it == plugInfoMap.end() )
			BOOST_THROW_EXCEPTION( std::logic_error( "Action not recognized: " + action ) );
		OFX::PluginFactory* factory = it->second._factory;

		// figure the actions
		if( action == kOfxActionLoad )
		{
			// call the support load function, param-less
			OFX::Private::loadAction();

			// call the plugin side load action, param-less
			factory->load();

			// got here, must be good
			stat = kOfxStatOK;
		}
		// figure the actions
		else if( action == kOfxActionUnload )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, true, true, true );

			// call the plugin side unload action, param-less, should be called, eve if the stat above failed!
			factory->unload();

			// call the support unload function, param-less
			OFX::Private::unloadAction( plugname );

			// got here, must be good
			stat = kOfxStatOK;
		}

		else if( action == kOfxActionDescribe )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// make the plugin descriptor
			ImageEffectDescriptor* desc = new ImageEffectDescriptor( handle );

			// validate the host
			OFX::Validation::validatePluginDescriptorProperties( fetchEffectProps( handle ) );

			//  and pass it to the plugin to do something with it

			factory->describe( *desc );

			// add it to our map
			gEffectDescriptors[plugname][eContextNone] = desc;

			// got here, must be good
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionDescribeInContext )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// make the plugin descriptor and pass it to the plugin to do something with it
			ImageEffectDescriptor* desc = new ImageEffectDescriptor( handle );

			// figure the context and map it to an enum
			std::string contextStr = inArgs.propGetString( kOfxImageEffectPropContext );
			EContext context    = mapContextStringToEnum( contextStr );

			// validate the host
			OFX::Validation::validatePluginDescriptorProperties( fetchEffectProps( handle ) );

			// call plugin descibe in context
			factory->describeInContext( *desc, context );

			// add it to our map
			gEffectDescriptors[plugname][context] = desc;

			// got here, must be good
			stat = kOfxStatOK;
		}
		else if( action == kOfxActionCreateInstance )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch the effect props to figure the context
			PropertySet effectProps = fetchEffectProps( handle );

			// get the context and turn it into an enum
			std::string str     = effectProps.propGetString( kOfxImageEffectPropContext );
			EContext context = mapContextStringToEnum( str );

			// make the image effect instance for this context
			/*ImageEffect *instance = */ factory->createInstance( handle, context );

			// validate the plugin handle's properties
			OFX::Validation::validatePluginInstanceProperties( fetchEffectProps( handle ) );

			// got here, must be good
			stat = kOfxStatOK;
		}
		else if( action == kOfxActionDestroyInstance )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch our pointer out of the props on the handle
			ImageEffect* instance = retrieveImageEffectPointer( handle );

			// kill it
			delete instance;

			// got here, must be good
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionRender )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the render action skin
			renderAction( handle, inArgs );

			// got here, must be good
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionBeginSequenceRender )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the begin render action skin
			beginSequenceRenderAction( handle, inArgs );
		}
		else if( action == kOfxImageEffectActionEndSequenceRender )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the begin render action skin
			endSequenceRenderAction( handle, inArgs );
		}
		else if( action == kOfxImageEffectActionIsIdentity )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false );

			// call the identity action, if it is, return OK
			if( isIdentityAction( handle, inArgs, outArgs ) )
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionGetRegionOfDefinition )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false );

			// call the rod action, return OK if it does something
			if( regionOfDefinitionAction( handle, inArgs, outArgs ) )
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionGetRegionsOfInterest )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false );

			// call the RoI action, return OK if it does something
			if( regionsOfInterestAction( handle, inArgs, outArgs, plugname ) )
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionGetFramesNeeded )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false );

			// call the frames needed action, return OK if it does something
			if( framesNeededAction( handle, inArgs, outArgs, plugname ) )
			stat = kOfxStatOK;
		}
		else if( action == kOfxImageEffectActionGetClipPreferences )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, false );

			// call the frames needed action, return OK if it does something
			if( clipPreferencesAction( handle, outArgs, plugname ) )
			stat = kOfxStatOK;
		}
		else if( action == kOfxActionPurgeCaches )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch our pointer out of the props on the handle
			ImageEffect* instance = retrieveImageEffectPointer( handle );

			// purge 'em
			instance->purgeCaches();
		}
		else if( action == kOfxActionSyncPrivateData )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch our pointer out of the props on the handle
			ImageEffect* instance = retrieveImageEffectPointer( handle );

			// and sync it
			instance->syncPrivateData();
		}
		else if( action == kOfxImageEffectActionGetTimeDomain )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, false );

			// call the instance changed action
			if( getTimeDomainAction( handle, outArgs ) )
			stat = kOfxStatOK;

			// fetch our pointer out of the props on the handle
			/*ImageEffect *instance = */ retrieveImageEffectPointer( handle );

		}
		else if( action == kOfxActionBeginInstanceChanged )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the instance changed action
			beginInstanceChangedAction( handle, inArgs );
		}
		else if( action == kOfxActionInstanceChanged )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the instance changed action
			instanceChangedAction( handle, inArgs );
		}
		else if( action == kOfxActionEndInstanceChanged )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true );

			// call the instance changed action
			endInstanceChangedAction( handle, inArgs );
		}
		else if( action == kOfxActionBeginInstanceEdit )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch our pointer out of the props on the handle
			ImageEffect* instance = retrieveImageEffectPointer( handle );

			// call the begin edit function
			instance->beginEdit();
		}
		else if( action == kOfxActionEndInstanceEdit )
		{
			checkMainHandles( actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true );

			// fetch our pointer out of the props on the handle
			ImageEffect* instance = retrieveImageEffectPointer( handle );

			// call the end edit function
			instance->endEdit();
		}
		else if( actionRaw )
		{
			OFX::Log::error( true, "Unknown action '%s'.", actionRaw );
		}
		else
		{
			OFX::Log::error( true, "Requested action was a null pointer." );
		}
	}
	catch( boost::exception& e )
	{
		std::cerr << tuttle::common::Color::get()->_error;
		std::cerr << "__________" << std::endl;
		/*
		[tuttle::exception::tag_devMessage*] = DPX: Unable to open file.
		[OFX::tag_ofxStatus*] = kOfxStatErrValue
		[boost::errinfo_file_name_*] = /datas/tmp/master32secsh01.0014.dpx
		*/
		if( const boost::error_info_sstream* const messageException = boost::get_error_info< tuttle::exception::user >(e) )
		{
			std::cerr << "Error: " << *messageException << std::endl;
		}
		else
		{
			std::cerr << "Error: " "No message." << std::endl;
		}
		if( const std::string* const filenameException = boost::get_error_info< ::boost::errinfo_file_name >(e) )
		{
			std::cerr << "filename: \"" << *filenameException << "\"" << std::endl;
		}

	#ifndef TUTTLE_PRODUCTION
		std::cerr << "__________" << std::endl;
		std::cerr << "* Caught boost::exception on action " << actionRaw << std::endl;
	#ifndef BOOST_EXCEPTION_DISABLE
		std::cerr << boost::diagnostic_information(e);
	#endif
//		std::cerr << "* inArgs: " << inArgs << std::endl;
		std::cerr << "----------" << std::endl;
		std::cerr << "* Backtrace" << std::endl;
		std::cerr << boost::trace(e);
	#endif
		std::cerr << "__________" << std::endl;
		std::cerr << tuttle::common::Color::get()->_std;
		
		/// @todo there is an assert in boost::get_error_info here. Why?
		if( const ::OfxStatus* status = boost::get_error_info< ::OFX::ofxStatus >( e ) )
		{
			stat = *status;
		}
		else
		{
			stat = kOfxStatFailed;
		}
	}

	// catch suite exceptions
	catch( OFX::Exception::Suite& e )
	{
		std::cerr << "Caught OFX::Exception::Suite (" << e.what() << ")" << std::endl;
		stat = e.status();
	}

	// catch host inadequate exceptions
	catch( OFX::Exception::HostInadequate& e )
	{
		std::cerr << "Caught OFX::Exception::HostInadequate (" << e.what() << ")" << std::endl;
		stat = kOfxStatErrMissingHostFeature;
	}

	// catch exception due to a property being unknown to the host, implies something wrong with host if not caught further down
	catch( OFX::Exception::PropertyUnknownToHost& e )
	{
		std::cerr << "Caught OFX::Exception::PropertyUnknownToHost (" << e.what() << ")" << std::endl;
		stat = kOfxStatErrMissingHostFeature;
	}

	// catch memory
	catch( std::bad_alloc& e )
	{
		std::cerr << "Caught std::bad_alloc (" << e.what() << ")" << std::endl;
		stat = kOfxStatErrMemory;
	}

	// catch a custom client exception, if defined
	#ifdef OFX_CLIENT_EXCEPTION_TYPE
	catch( OFX_CLIENT_EXCEPTION_TYPE& e )
	{
		std::cerr << "Caught OFX_CLIENT_EXCEPTION (" << e.what() << ")" << std::endl;
		stat = OFX_CLIENT_EXCEPTION_HANDLER( e, plugname );
	}
	#endif

	// catch all exceptions
	catch( std::exception& e )
	{
		std::cerr << "Caught std::exception on action " << actionRaw << " (" << e.what() << ")" << std::endl;
		stat = kOfxStatFailed;
	}

	// Catch anything else, unknown
	catch( ... )
	{
		std::cerr << "Caught Unknown exception (file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
		stat = kOfxStatFailed;
	}

	OFX::Log::outdent();
	OFX::Log::print( "STOP mainEntry (%s)\n", actionRaw );
	return stat;
}

/** @brief The plugin function that gets passed the host structure. */
void setHost( OfxHost* host )
{
	gHost = host;
}

OfxPlugInfo generatePlugInfo( PluginFactory* factory )
{
	std::auto_ptr<OfxPlugin> ofxPlugin( new OfxPlugin() );
	ofxPlugin->pluginApi          = kOfxImageEffectPluginApi;
	ofxPlugin->apiVersion         = kOfxImageEffectPluginApiVersion;
	ofxPlugin->pluginIdentifier   = factory->getID().c_str();
	ofxPlugin->pluginVersionMajor = factory->getMajorVersion();
	ofxPlugin->pluginVersionMinor = factory->getMinorVersion();
	ofxPlugin->setHost            = OFX::Private::setHost;
	ofxPlugin->mainEntry          = factory->getMainEntry();
	return OfxPlugInfo( factory, ofxPlugin.release() );
}

} // namespace Private

/** @brief Fetch's a suite from the host and logs errors */
void* fetchSuite( const char* suiteName, int suiteVersion, bool optional )
{
	void* suite = Private::gHost->fetchSuite( Private::gHost->host, suiteName, suiteVersion );

	if( suite == 0 )
	{
		if( optional )
			OFX::Log::warning( suite == 0, "Could not fetch the optional suite '%s' version %d.", suiteName, suiteVersion );
		else
			OFX::Log::error( suite == 0, "Could not fetch the mandatory suite '%s' version %d.", suiteName, suiteVersion );
	}
	if( !optional && suite == 0 )
	BOOST_THROW_EXCEPTION( OFX::Exception::HostInadequate( suiteName ) );
	return suite;
}

} // namespace OFX

namespace OFX {
namespace Plugin {
void getPluginIDs( OFX::PluginFactoryArray& ids );
}
}


void init()
{
	using namespace OFX;
	using namespace OFX::Private;
	if( gHasInit )
	{
		return;
	}

	::OFX::Plugin::getPluginIDs( plugIDs );
	ofxPlugs.reserve( plugIDs.size() );

	BOOST_FOREACH( PluginFactory* factory, plugIDs )
	{
		OfxPlugInfo info = generatePlugInfo( factory );
		const std::string newID = factory->getUID();
		plugInfoMap[newID] = info;
		ofxPlugs.push_back( info._plug );
	}
	gHasInit = true;
}


/** @brief, mandated function returning the number of plugins, which is always 1 */
OfxExport int OfxGetNumberOfPlugins( void )
{
	init();
	return static_cast<int>( OFX::plugIDs.size() );
}

/** @brief, mandated function returning the nth plugin
*
* We call the plugin side defined OFX::Plugin::getPluginID function to find out what to set.
*/
OfxExport OfxPlugin* OfxGetPlugin( int nth )
{
	init();
	const int numPlugs = static_cast<int>( OFX::Private::plugInfoMap.size() );
	OFX::Log::error( nth >= numPlugs, "Host attempted to get plugin %d, when there is only %d plugin(s).", nth, numPlugs );
	BOOST_ASSERT( nth < numPlugs  );
	OFX::Log::error( OFX::Private::ofxPlugs[nth] == NULL, "Host attempted to get plugin %d, but it is unloaded.", nth );
	BOOST_ASSERT( OFX::Private::ofxPlugs[nth] != NULL );
	return OFX::Private::ofxPlugs[nth];
}

