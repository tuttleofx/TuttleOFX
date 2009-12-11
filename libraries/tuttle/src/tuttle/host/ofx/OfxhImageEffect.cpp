/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
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

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhPropertySuite.hpp"
#include "OfxhClip.hpp"
#include "OfxhParam.hpp"
#include "OfxhMemory.hpp"
#include "OfxhImageEffect.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"
#include "OfxhImageEffectAPI.hpp"
#include "OfxhUtilities.hpp"

#include <tuttle/host/core/Core.hpp>

#include <cstring>
#include <cstdarg>
#include <cmath>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

/// properties common on an effect and a descriptor
static property::OfxhPropSpec effectDescriptorStuff[] = {
	/* name                                 type                   dim. r/o default value */
	{ kOfxPropType, property::eString, 1, true, kOfxTypeImageEffect },
	{ kOfxPropName, property::eString, 1, false, "UNIQUE_NAME_NOT_SET" },
	{ kOfxPropLabel, property::eString, 1, false, "" },
	{ kOfxPropShortLabel, property::eString, 1, false, "" },
	{ kOfxPropLongLabel, property::eString, 1, false, "" },
	{ kOfxImageEffectPropSupportedContexts, property::eString, 0, false, "" },
	{ kOfxImageEffectPluginPropGrouping, property::eString, 1, false, "" },
	{ kOfxImageEffectPluginPropSingleInstance, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPluginRenderThreadSafety, property::eString, 1, false, kOfxImageEffectRenderInstanceSafe },
	{ kOfxImageEffectPluginPropHostFrameThreading, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPluginPropOverlayInteractV1, property::ePointer, 1, false, NULL },
	{ kOfxImageEffectPropSupportsMultiResolution, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropSupportsTiles, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropTemporalClipAccess, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropSupportedPixelDepths, property::eString, 0, false, "" },
	{ kOfxImageEffectPluginPropFieldRenderTwiceAlways, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropSupportsMultipleClipDepths, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropSupportsMultipleClipPARs, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropClipPreferencesSlaveParam, property::eString, 0, false, "" },
	{ kOfxPluginPropFilePath, property::eString, 1, true, "" },
	{ 0 }
};

//
// Base
//
Base::Base( const property::OfxhSet& set )
	: _properties( set )
{}

Base::Base( const property::OfxhPropSpec* propSpec )
	: _properties( propSpec )
{}

Base::~Base() {}

/// obtain a handle on this for passing to the C api

OfxImageEffectHandle Base::getHandle() const
{
	return ( OfxImageEffectHandle ) this;
}

/// name of the clip

const std::string& Base::getShortLabel() const
{
	const std::string& s = _properties.getStringProperty( kOfxPropShortLabel );

	if( s == "" )
	{
		const std::string& s2 = getLabel();
		if( s2 == "" )
		{
			return getName();
		}
	}
	return s;
}

/// name of the clip

const std::string& Base::getLabel() const
{
	const std::string& s = _properties.getStringProperty( kOfxPropLabel );

	if( s == "" )
	{
		return getName();
	}
	return s;
}

/// name of the clip
const std::string& Base::getName() const
{
	return _properties.getStringProperty( kOfxPropName );
}

/// name of the clip
void Base::setName( const std::string& name )
{
	_properties.setStringProperty( kOfxPropName, name );
}

/// name of the clip

const std::string& Base::getLongLabel() const
{
	const std::string& s = _properties.getStringProperty( kOfxPropLongLabel );

	if( s == "" )
	{
		const std::string& s2 = getLabel();
		if( s2 == "" )
		{
			return getName();
		}
	}
	return s;
}

/// is the given context supported

bool Base::isContextSupported( const std::string& s ) const
{
	return _properties.findStringPropValueIndex( kOfxImageEffectPropSupportedContexts, s ) != -1;
}

/// what is the name of the group the plug-in belongs to

const std::string& Base::getPluginGrouping() const
{
	return _properties.getStringProperty( kOfxImageEffectPluginPropGrouping );
}

/// is the effect single instance

bool Base::isSingleInstance() const
{
	return _properties.getIntProperty( kOfxImageEffectPluginPropSingleInstance ) != 0;
}

/// what is the thread safety on this effect

const std::string& Base::getRenderThreadSafety() const
{
	return _properties.getStringProperty( kOfxImageEffectPluginRenderThreadSafety );
}

/// should the host attempt to managed multi-threaded rendering if it can
/// via tiling or some such

bool Base::getHostFrameThreading() const
{
	return _properties.getIntProperty( kOfxImageEffectPluginPropHostFrameThreading ) != 0;
}

/// get the overlay interact main entry if it exists

OfxPluginEntryPoint* Base::getOverlayInteractMainEntry() const
{
	return ( OfxPluginEntryPoint* )( _properties.getPointerProperty( kOfxImageEffectPluginPropOverlayInteractV1 ) );
}

/// does the effect support images of differing sizes

bool Base::supportsMultiResolution() const
{
	return _properties.getIntProperty( kOfxImageEffectPropSupportsMultiResolution ) != 0;
}

/// does the effect support tiled rendering

bool Base::supportsTiles() const
{
	return _properties.getIntProperty( kOfxImageEffectPropSupportsTiles ) != 0;
}

/// does this effect need random temporal access

bool Base::temporalAccess() const
{
	return _properties.getIntProperty( kOfxImageEffectPropTemporalClipAccess ) != 0;
}

/// is the given RGBA/A pixel depth supported by the effect

bool Base::isPixelDepthSupported( const std::string& s ) const
{
	return _properties.findStringPropValueIndex( kOfxImageEffectPropSupportedPixelDepths, s ) != -1;
}

/// when field rendering, does the effect need to be called
/// twice to render a frame in all Base::circumstances (with different fields)

bool Base::fieldRenderTwiceAlways() const
{
	return _properties.getIntProperty( kOfxImageEffectPluginPropFieldRenderTwiceAlways ) != 0;
}

/// does the effect support multiple clip depths

bool Base::supportsMultipleClipDepths() const
{
	return _properties.getIntProperty( kOfxImageEffectPropSupportsMultipleClipDepths ) != 0;
}

/// does the effect support multiple clip pixel aspect ratios

bool Base::supportsMultipleClipPARs() const
{
	return _properties.getIntProperty( kOfxImageEffectPropSupportsMultipleClipPARs ) != 0;
}

/// does changing the named param re-tigger a clip preferences action

bool Base::isClipPreferencesSlaveParam( const std::string& s ) const
{
	return _properties.findStringPropValueIndex( kOfxImageEffectPropClipPreferencesSlaveParam, s ) != -1;
}

////////////////////////////////////////////////////////////////////////////////
// descriptor

Descriptor::Descriptor( OfxhPlugin* plug )
	: Base( effectDescriptorStuff ),
	_plugin( plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, plug->getBinary()->getBundlePath() );
	tuttle::host::core::Core::instance().getHost().initDescriptor( this );
}

Descriptor::Descriptor( const Descriptor& other, OfxhPlugin* plug )
	: Base( other._properties ),
	_plugin( plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, plug->getBinary()->getBundlePath() );
	tuttle::host::core::Core::instance().getHost().initDescriptor( this );
}

Descriptor::Descriptor( const std::string& bundlePath, OfxhPlugin* plug )
	: Base( effectDescriptorStuff ),
	_plugin( plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, bundlePath );
	tuttle::host::core::Core::instance().getHost().initDescriptor( this );
}

Descriptor::~Descriptor()
{}

/// create a new clip and add this to the clip map

attribute::OfxhClipImageDescriptor* Descriptor::defineClip( const std::string& name )
{
	attribute::OfxhClipImageDescriptor* c = new attribute::OfxhClipImageDescriptor( name );

	_clips[name] = c;
	_clipsByOrder.push_back( c );
	return c;
}

/// @warning tuttle some modifs here, doc needs update
/// get the interact description, this will also call describe on the interact
void Descriptor::initOverlayDescriptor( int bitDepthPerComponent, bool hasAlpha )
{
	if( _overlayDescriptor.getState() == interact::eUninitialised )
	{
		// OK, we need to describe it, set the entry point and describe away
		_overlayDescriptor.setEntryPoint( getOverlayInteractMainEntry() );
		_overlayDescriptor.describe( bitDepthPerComponent, hasAlpha );
	}
}

void Descriptor::addClip( const std::string& name, attribute::OfxhClipImageDescriptor* clip )
{
	_clips[name] = clip;
	_clipsByOrder.push_back( clip );
}

static const property::OfxhPropSpec effectInstanceStuff[] = {
	/* name                                 type                   dim.   r/o    default value */
	{ kOfxPropType, property::eString, 1, true, kOfxTypeImageEffectInstance },
	{ kOfxPropName, property::eString, 1, false, "UNIQUE_NAME_NOT_SET" },
	{ kOfxImageEffectPropContext, property::eString, 1, true, "" },
	{ kOfxPropInstanceData, property::ePointer, 1, false, NULL },
	{ kOfxImageEffectPropProjectSize, property::eDouble, 2, true, "0" },
	{ kOfxImageEffectPropProjectOffset, property::eDouble, 2, true, "0" },
	{ kOfxImageEffectPropProjectExtent, property::eDouble, 2, true, "0" },
	{ kOfxImageEffectPropProjectPixelAspectRatio, property::eDouble, 1, true, "0" },
	{ kOfxImageEffectInstancePropEffectDuration, property::eDouble, 1, true, "0" },
	{ kOfxImageEffectInstancePropSequentialRender, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropFrameRate, property::eDouble, 1, true, "0" },
	{ kOfxPropIsInteractive, property::eInt, 1, true, "0" },
	{ 0 }
};

Instance::Instance( const OfxhImageEffectPlugin* plugin,
                    const Descriptor&            descriptor,
                    const std::string&           context,
                    bool                         interactive )
	: Base( effectInstanceStuff ),
	_plugin( plugin ),
	_context( context ),
	_descriptor( descriptor ),
	_interactive( interactive ),
	_created( false ),
	_continuousSamples( false ),
	_frameVarying( false ),
	_outputFrameRate( 24 )
{
	int i = 0;

	_properties.setChainedSet( &descriptor.getProperties() );

	_properties.setStringProperty( kOfxImageEffectPropContext, context );
	_properties.setIntProperty( kOfxPropIsInteractive, interactive );

	// copy is sequential over
	bool sequential = descriptor.getProperties().getIntProperty( kOfxImageEffectInstancePropSequentialRender ) != 0;
	_properties.setIntProperty( kOfxImageEffectInstancePropSequentialRender, sequential );

	while( effectInstanceStuff[i].name )
	{
		// don't set hooks for context or isinteractive
		if( strcmp( effectInstanceStuff[i].name, kOfxImageEffectPropContext ) ||
		    strcmp( effectInstanceStuff[i].name, kOfxPropIsInteractive ) ||
		    strcmp( effectInstanceStuff[i].name, kOfxImageEffectInstancePropSequentialRender ) )
		{
			const property::OfxhPropSpec& spec = effectInstanceStuff[i];

			switch( spec.type )
			{
				case property::eDouble:
					_properties.setGetHook( spec.name, this );
					break;
				default:
					break;
			}
		}
		++i;
	}
}

Instance::Instance( const Instance& other )
	: Base( other.getProperties() ),
	attribute::OfxhParamSet( other ),
	attribute::OfxhClipImageSet( other ),
	_plugin( other.getPlugin() ),
	_context( other.getContext() ),
	_descriptor( other.getDescriptor() ),
	_interactive( other._interactive ),
	_created( other._created ),
	_continuousSamples( other._continuousSamples ),
	_frameVarying( other._frameVarying ),
	_outputFrameRate( other._outputFrameRate )
{}

/// called after construction to populate clips and params
OfxStatus Instance::populate()
{
	try
	{
		populateClips( _descriptor );
	}
	catch( core::exception::LogicError& e )
	{
		COUT_EXCEPTION( e );
		return kOfxStatFailed;
	}

	try
	{
		populateParams( _descriptor );
	}
	catch( core::exception::LogicError& e )
	{
		COUT_EXCEPTION( e );
		return kOfxStatFailed;
	}

	return kOfxStatOK;
}

void Instance::populateParams( const imageEffect::Descriptor& descriptor ) throw( core::exception::LogicError )
{

	const std::list<attribute::OfxhParamDescriptor*>& map = _descriptor.getParamList();

	std::map<std::string, attribute::OfxhParam*> parameters;

	// Create parameters on their own groups
	for( std::list<attribute::OfxhParamDescriptor*>::const_iterator it = map.begin(), itEnd = map.end();
	     it != itEnd;
	     ++it )
	{
		attribute::OfxhParamSet* setInstance = this;
		// SetInstance where the childrens param instances will be added
		attribute::OfxhParamDescriptor* descriptor = ( *it );

		// get the param descriptor
		if( !descriptor )
			throw core::exception::LogicError( kOfxStatErrValue );

		// name and parentName of the parameter
		std::string name       = descriptor->getName();
		std::string parentName = descriptor->getParentName();

		if( parentName != "" )
		{
			attribute::OfxhParamGroup* parentGroup = dynamic_cast<attribute::OfxhParamGroup*>( parameters[parentName] );
			if( parentGroup )
			{
				setInstance = parentGroup->getChildrens();
			}
		}
		else
			setInstance = this;

		// get a param instance from a param descriptor. Param::Instance is automatically added into the setInstance provided.
		attribute::OfxhParam* instance = newParam( *descriptor );
		/// @todo set the groups of the ParamInstance !!!
		parameters[name] = instance;

	}

}

/**
 * @todo check clip ? check hash ? etc.
 */
bool Instance::operator==( const Instance& other )
{
	bool result;

	result = getParamList() == other.getParamList();
	// if( !result )
	//  return result;
	// result = std::equal( getClipList().begin(), getClipList().end(), other.getClipList().begin() );
	return result;
}

/// implemented for Param::SetDescriptor

property::OfxhSet& Instance::getParamSetProps()
{
	return _properties;
}

// do nothing
size_t Instance::getDimension( const std::string& name ) const OFX_EXCEPTION_SPEC
{
	fprintf( stderr, "failing in %s with name=%s\n", "__PRETTY_FUNCTION__", name.c_str() );
	throw property::OfxhException( kOfxStatErrMissingHostFeature );
}

size_t Instance::upperGetDimension( const std::string& name )
{
	return _properties.getDimension( name );
}

void Instance::notify( const std::string& name, bool singleValue, int indexOrN ) OFX_EXCEPTION_SPEC
{
	fprintf( stderr, "failing in %s\n", "__PRETTY_FUNCTION__" );
}

// don't know what to do

void Instance::reset( const std::string& name ) OFX_EXCEPTION_SPEC
{
	fprintf( stderr, "failing in %s\n", "__PRETTY_FUNCTION__" );
	throw property::OfxhException( kOfxStatErrMissingHostFeature );
}

// get the virutals for viewport size, pixel scale, background colour

double Instance::getDoubleProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxImageEffectPropProjectSize )
	{
		if( index >= 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectSize( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectOffset )
	{
		if( index >= 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectOffset( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectExtent )
	{
		if( index >= 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectExtent( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectPixelAspectRatio )
	{
		if( index >= 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		return getProjectPixelAspectRatio();
	}
	else if( name == kOfxImageEffectInstancePropEffectDuration )
	{
		if( index >= 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		return getEffectDuration();
	}
	else if( name == kOfxImageEffectPropFrameRate )
	{
		if( index >= 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		return getFrameRate();
	}
	else
		throw property::OfxhException( kOfxStatErrUnknown );
}

void Instance::getDoublePropertyN( const std::string& name, double* first, int n ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxImageEffectPropProjectSize )
	{
		if( n > 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getProjectSize( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectOffset )
	{
		if( n > 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getProjectOffset( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectExtent )
	{
		if( n > 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getProjectExtent( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectPixelAspectRatio )
	{
		if( n > 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		*first = getProjectPixelAspectRatio();
	}
	else if( name == kOfxImageEffectInstancePropEffectDuration )
	{
		if( n > 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		*first = getEffectDuration();
	}
	else if( name == kOfxImageEffectPropFrameRate )
	{
		if( n > 1 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		*first = getFrameRate();
	}
	else
		throw property::OfxhException( kOfxStatErrUnknown );
}

Instance::~Instance()
{
	// destroy the instance, only if succesfully created
	if( _created )
	{
		mainEntry( kOfxActionDestroyInstance, this->getHandle(), 0, 0 );
	}
}

/// this is used to populate with any extra action in arguments that may be needed

void Instance::setCustomInArgs( const std::string& action, property::OfxhSet& inArgs ) {}

/// this is used to populate with any extra action out arguments that may be needed

void Instance::setCustomOutArgs( const std::string& action, property::OfxhSet& outArgs ) {}

/// this is used to populate with any extra action out arguments that may be needed

void Instance::examineOutArgs( const std::string& action, OfxStatus, const property::OfxhSet& outArgs ) {}

/// check for connection

bool Instance::checkClipConnectionStatus() const
{
	std::map<std::string, attribute::OfxhClipImage*>::const_iterator i;
	for( i = _clips.begin(); i != _clips.end(); ++i )
	{
		if( !i->second->isOptional() && !i->second->getConnected() )
		{
			return false;
		}
	}
	return true;
}

// override this to make processing abort, return 1 to abort processing

int Instance::abort()
{
	return 0;
}

// override this to use your own memory instance - must inherrit from memory::instance

memory::OfxhMemory* Instance::newMemoryInstance( size_t nBytes )
{
	return 0;
}

// return an memory::instance calls makeMemoryInstance that can be overriden

memory::OfxhMemory* Instance::imageMemoryAlloc( size_t nBytes )
{
	memory::OfxhMemory* instance = newMemoryInstance( nBytes );

	if( instance )
		return instance;
	else
	{
		memory::OfxhMemory* instance = new memory::OfxhMemory;
		instance->alloc( nBytes );
		return instance;
	}
}

// call the effect entry point

OfxStatus Instance::mainEntry( const char*        action,
                               const void*        handle,
                               property::OfxhSet* inArgs,
                               property::OfxhSet* outArgs )
{
	if( _plugin )
	{
		const OfxhPluginHandle* pHandle = _plugin->getPluginHandle();
		if( pHandle )
		{
			const OfxPlugin* ofxPlugin = pHandle->getOfxPlugin();
			if( ofxPlugin )
			{

				OfxPropertySetHandle inHandle = 0;
				if( inArgs )
				{
					setCustomInArgs( action, *inArgs );
					inHandle = inArgs->getHandle();
				}

				OfxPropertySetHandle outHandle = 0;
				if( outArgs )
				{
					setCustomOutArgs( action, *outArgs );
					outHandle = outArgs->getHandle();
				}

				OfxStatus stat = ofxPlugin->mainEntry( action, handle, inHandle, outHandle );

				if( outArgs )
					examineOutArgs( action, stat, *outArgs );

				return stat;
			}
			return kOfxStatFailed;
		}
		return kOfxStatFailed;
	}
	return kOfxStatFailed;
}

// create a clip instance

OfxStatus Instance::createInstanceAction()
{
	/// we need to init the clips before we call create instance incase
	/// they try and fetch something in create instance, which they are allowed
	setDefaultClipPreferences();

	// now tell the plug-in to create instance
	OfxStatus status = mainEntry( kOfxActionCreateInstance, this->getHandle(), 0, 0 );

	if( status == kOfxStatOK )
	{
		_created = true;
	}

	return status;
}

// begin/change/end instance changed

OfxStatus Instance::beginInstanceChangedAction( const std::string& why )
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	return mainEntry( kOfxActionBeginInstanceChanged, this->getHandle(), &inArgs, 0 );
}

OfxStatus Instance::paramInstanceChangedAction( const std::string& paramName,
                                                const std::string& why,
                                                OfxTime            time,
                                                OfxPointD          renderScale )
{
	try
	{
		/*attribute::OfxhParam& param = */getParam( paramName );

		if( isClipPreferencesSlaveParam( paramName ) )
			_clipPrefsDirty = true;

		property::OfxhPropSpec stuff[] = {
			{ kOfxPropType, property::eString, 1, true, kOfxTypeParameter },
			{ kOfxPropName, property::eString, 1, true, paramName.c_str() },
			{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
			{ kOfxPropTime, property::eDouble, 1, true, "0" },
			{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
			{ 0 }
		};

		property::OfxhSet inArgs( stuff );

		// add the second dimension of the render scale
		inArgs.setDoubleProperty( kOfxPropTime, time );

		inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

		return mainEntry( kOfxActionInstanceChanged, this->getHandle(), &inArgs, 0 );

	}
	catch( core::exception::LogicError& e )
	{
		COUT_EXCEPTION( e );
		return kOfxStatFailed;
	}
	catch(... )
	{
		COUT_ERROR( "Exception..." );
		return kOfxStatFailed;
	}
}

OfxStatus Instance::clipInstanceChangedAction( const std::string& clipName,
                                               const std::string& why,
                                               OfxTime            time,
                                               OfxPointD          renderScale )
{
	_clipPrefsDirty = true;
	std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.find( clipName );
	if( it != _clips.end() )
		return ( it->second )->instanceChangedAction( why, time, renderScale );
	else
		return kOfxStatFailed;
}

OfxStatus Instance::endInstanceChangedAction( const std::string& why )
{
	property::OfxhPropSpec whyStuff[] = {
		{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
		{ 0 }
	};

	property::OfxhSet inArgs( whyStuff );

	return mainEntry( kOfxActionEndInstanceChanged, this->getHandle(), &inArgs, 0 );
}

// purge your caches

OfxStatus Instance::purgeCachesAction()
{
	return mainEntry( kOfxActionPurgeCaches, this->getHandle(), 0, 0 );
}

// sync your private data

OfxStatus Instance::syncPrivateDataAction()
{
	return mainEntry( kOfxActionSyncPrivateData, this->getHandle(), 0, 0 );
}

// begin/end edit instance

OfxStatus Instance::beginInstanceEditAction()
{
	return mainEntry( kOfxActionBeginInstanceEdit, this->getHandle(), 0, 0 );
}

OfxStatus Instance::endInstanceEditAction()
{
	return mainEntry( kOfxActionEndInstanceEdit, this->getHandle(), 0, 0 );
}

OfxStatus Instance::beginRenderAction( OfxTime   startFrame,
                                       OfxTime   endFrame,
                                       OfxTime   step,
                                       bool      interactive,
                                       OfxPointD renderScale )
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageEffectPropFrameStep, property::eDouble, 1, true, "0" },
		{ kOfxPropIsInteractive, property::eInt, 1, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	// set up second dimension for frame range and render scale
	inArgs.setDoubleProperty( kOfxImageEffectPropFrameRange, startFrame, 0 );
	inArgs.setDoubleProperty( kOfxImageEffectPropFrameRange, endFrame, 1 );

	inArgs.setDoubleProperty( kOfxImageEffectPropFrameStep, step );

	inArgs.setIntProperty( kOfxPropIsInteractive, interactive );

	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	return mainEntry( kOfxImageEffectActionBeginSequenceRender, this->getHandle(), &inArgs, 0 );
}

OfxStatus Instance::renderAction( OfxTime            time,
                                  const std::string& field,
                                  const OfxRectI&    renderRoI,
                                  OfxPointD          renderScale )
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxPropTime, property::eDouble, 1, true, "0" },
		{ kOfxImageEffectPropFieldToRender, property::eString, 1, true, "" },
		{ kOfxImageEffectPropRenderWindow, property::eInt, 4, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	inArgs.setStringProperty( kOfxImageEffectPropFieldToRender, field );
	inArgs.setDoubleProperty( kOfxPropTime, time );
	inArgs.setIntPropertyN( kOfxImageEffectPropRenderWindow, &renderRoI.x1, 4 );
	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	return mainEntry( kOfxImageEffectActionRender, this->getHandle(), &inArgs, 0 );
}

OfxStatus Instance::endRenderAction( OfxTime   startFrame,
                                     OfxTime   endFrame,
                                     OfxTime   step,
                                     bool      interactive,
                                     OfxPointD renderScale )
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageEffectPropFrameStep, property::eDouble, 1, true, "0" },
		{ kOfxPropIsInteractive, property::eInt, 1, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	inArgs.setDoubleProperty( kOfxImageEffectPropFrameStep, step );

	inArgs.setDoubleProperty( kOfxImageEffectPropFrameRange, startFrame, 0 );
	inArgs.setDoubleProperty( kOfxImageEffectPropFrameRange, endFrame, 1 );
	inArgs.setIntProperty( kOfxPropIsInteractive, interactive );
	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	return mainEntry( kOfxImageEffectActionEndSequenceRender, this->getHandle(), &inArgs, 0 );
}

/// calculate the default rod for this effect instance

OfxRectD Instance::calcDefaultRegionOfDefinition( OfxTime   time,
                                                  OfxPointD renderScale )
{
	OfxRectD rod;

	// figure out the default contexts
	if( _context == kOfxImageEffectContextGenerator )
	{
		// generator is the extent
		rod.x1 = rod.y1 = 0;
		getProjectExtent( rod.x2, rod.y2 );
		//@OFX_TODO: le mal est ici... pour les Read ca se passe par la...
	}
	else if( _context == kOfxImageEffectContextFilter ||
	         _context == kOfxImageEffectContextPaint )
	{
		try
		{
			// filter and paint default to the input clip
			attribute::OfxhClipImage& clip = getClip( kOfxImageEffectSimpleSourceClipName );
			rod = clip.getRegionOfDefinition( time );
		}
		catch( core::exception::LogicError& e )
		{
			COUT_EXCEPTION( e );
		}
	}
	else if( _context == kOfxImageEffectContextTransition )
	{
		try
		{
			// transition is the union of the two clips
			attribute::OfxhClipImage& clipFrom = getClip( kOfxImageEffectTransitionSourceFromClipName );
			attribute::OfxhClipImage& clipTo   = getClip( kOfxImageEffectTransitionSourceToClipName );
			rod = clipFrom.getRegionOfDefinition( time );
			rod = Union( rod, clipTo.getRegionOfDefinition( time ) );
		}
		catch( core::exception::LogicError& e )
		{
			COUT_EXCEPTION( e );
		}
	}
	else if( _context == kOfxImageEffectContextGeneral )
	{
		// general context is the union of all the non optional clips
		bool gotOne = false;
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			attribute::OfxhClipImage* clip = it->second;
			if( !clip->isOutput() && !clip->isOptional() )
			{
				if( !gotOne )
					rod = clip->getRegionOfDefinition( time );
				else
					rod = Union( rod, clip->getRegionOfDefinition( time ) );
				gotOne = true;
			}
		}

		if( !gotOne )
		{
			/// no non optionals? then be the extent
			rod.x1 = rod.y1 = 0;
			getProjectExtent( rod.x2, rod.y2 );
		}

	}
	else if( _context == kOfxImageEffectContextRetimer )
	{
		// retimer
		try
		{
			attribute::OfxhClipImage& clip        = getClip( kOfxImageEffectSimpleSourceClipName );
			/*attribute::ParamDoubleInstance& param = */dynamic_cast<attribute::ParamDoubleInstance&>( getParam( kOfxImageEffectRetimerParamName ) );
			rod = clip.getRegionOfDefinition( floor( time ) );
			rod = Union( rod, clip.getRegionOfDefinition( floor( time ) + 1 ) );
		}
		catch( core::exception::LogicError& e )
		{
			COUT_EXCEPTION( e );
		}
		catch(... )
		{
			COUT_ERROR( "Exception." );
		}
	}

	return rod;
}

////////////////////////////////////////////////////////////////////////////////
// RoD call

OfxStatus Instance::getRegionOfDefinitionAction( OfxTime   time,
                                                 OfxPointD renderScale,
                                                 OfxRectD& rod )
{
	property::OfxhPropSpec inStuff[] = {
		{ kOfxPropTime, property::eDouble, 1, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhPropSpec outStuff[] = {
		{ kOfxImageEffectPropRegionOfDefinition, property::eDouble, 4, false, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( inStuff );
	property::OfxhSet outArgs( outStuff );

	inArgs.setDoubleProperty( kOfxPropTime, time );

	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	OfxStatus stat = mainEntry( kOfxImageEffectActionGetRegionOfDefinition,
	                            this->getHandle(),
	                            &inArgs,
	                            &outArgs );

	if( stat == kOfxStatOK )
	{
		outArgs.getDoublePropertyN( kOfxImageEffectPropRegionOfDefinition, &rod.x1, 4 );
	}
	else if( stat == kOfxStatReplyDefault )
	{
		rod = calcDefaultRegionOfDefinition( time, renderScale );
	}

	return stat;
}

/// get the region of interest for each input and return it in the given std::map

OfxStatus Instance::getRegionOfInterestAction( OfxTime time,
                                               OfxPointD renderScale,
                                               const OfxRectD& roi,
                                               std::map<attribute::OfxhClipImage*, OfxRectD>& rois )
{
	OfxStatus stat = kOfxStatReplyDefault;

	// reset the map
	rois.clear();

	if( !supportsTiles() )
	{
		/// No tiling support on the effect at all. So set the roi of each input clip to be the RoD of that clip.
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			if( !it->second->isOutput() || getContext() == kOfxImageEffectContextGenerator )
			{
				// OFX_TODO
				OfxRectD roi = it->second->getRegionOfDefinition( time );
				rois[it->second] = roi;
			}
		}
		stat = kOfxStatOK;
	}
	else
	{
		/// set up the in args
		static property::OfxhPropSpec inStuff[] = {
			{ kOfxPropTime, property::eDouble, 1, true, "0" },
			{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
			{ kOfxImageEffectPropRegionOfInterest, property::eDouble, 4, true, 0 },
			{ 0 }
		};
		property::OfxhSet inArgs( inStuff );

		inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );
		inArgs.setDoubleProperty( kOfxPropTime, time );
		inArgs.setDoublePropertyN( kOfxImageEffectPropRegionOfInterest, &roi.x1, 4 );

		property::OfxhSet outArgs;
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			if( !it->second->isOutput() || getContext() == kOfxImageEffectContextGenerator )
			{
				property::OfxhPropSpec s;
				std::string name = "OfxImageClipPropRoI_" + it->first;

				s.name         = name.c_str();
				s.type         = property::eDouble;
				s.dimension    = 4;
				s.readonly     = false;
				s.defaultValue = "";
				outArgs.createProperty( s );

				/// initialise to the default
				outArgs.setDoublePropertyN( s.name, &roi.x1, 4 );
			}
		}

		/// call the action
		stat = mainEntry( kOfxImageEffectActionGetRegionsOfInterest,
		                  this->getHandle(),
		                  &inArgs,
		                  &outArgs );

		/// set the thing up
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			if( !it->second->isOutput() || getContext() == kOfxImageEffectContextGenerator )
			{
				OfxRectD rod = it->second->getRegionOfDefinition( time );
				if( it->second->supportsTiles() )
				{
					std::string name = "OfxImageClipPropRoI_" + it->first;
					OfxRectD thisRoi;
					thisRoi.x1 = outArgs.getDoubleProperty( name, 0 );
					thisRoi.y1 = outArgs.getDoubleProperty( name, 1 );
					thisRoi.x2 = outArgs.getDoubleProperty( name, 2 );
					thisRoi.y2 = outArgs.getDoubleProperty( name, 3 );

					/// and clamp it to the clip's rod
					thisRoi          = Clamp( thisRoi, rod );
					rois[it->second] = thisRoi;
				}
				else
				{
					/// not supporting tiles on this input, so set it to the rod
					rois[it->second] = rod;
				}
			}
		}
	}

	return stat;
}

////////////////////////////////////////////////////////////////////////////////
/// see how many frames are needed from each clip to render the indicated frame

OfxStatus Instance::getFrameNeededAction( OfxTime   time,
                                          RangeMap& rangeMap )
{
	OfxStatus stat = kOfxStatReplyDefault;
	property::OfxhSet outArgs;

	if( temporalAccess() )
	{
		property::OfxhPropSpec inStuff[] = {
			{ kOfxPropTime, property::eDouble, 1, true, "0" },
			{ 0 }
		};
		property::OfxhSet inArgs( inStuff );
		inArgs.setDoubleProperty( kOfxPropTime, time );

		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			if( !it->second->isOutput() )
			{
				property::OfxhPropSpec s;
				std::string name = "OfxImageClipPropFrameRange_" + it->first;

				s.name         = name.c_str();
				s.type         = property::eDouble;
				s.dimension    = 0;
				s.readonly     = false;
				s.defaultValue = "";
				outArgs.createProperty( s );
				/// intialise it to the current frame
				outArgs.setDoubleProperty( name, time, 0 );
				outArgs.setDoubleProperty( name, time, 1 );
			}
		}

		stat = mainEntry( kOfxImageEffectActionGetFramesNeeded,
		                  this->getHandle(),
		                  &inArgs,
		                  &outArgs );
	}

	OfxRangeD defaultRange;
	defaultRange.min     =
	    defaultRange.max = time;

	for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
	     it != _clips.end();
	     it++ )
	{
		attribute::OfxhClipImage* clip = it->second;

		if( !clip->isOutput() )
		{
			if( stat != kOfxStatOK )
			{
				rangeMap[clip].push_back( defaultRange );
			}
			else
			{
				std::string name = "OfxImageClipPropFrameRange_" + it->first;

				int nRanges = outArgs.getDimension( name );
				if( nRanges % 2 != 0 )
					return kOfxStatFailed;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // bad! needs to be divisible by 2

				if( nRanges == 0 )
				{
					rangeMap[clip].push_back( defaultRange );
				}
				else
				{
					for( int r = 0; r < nRanges; )
					{
						double min = outArgs.getDoubleProperty( name, r );
						double max = outArgs.getDoubleProperty( name, r + 1 );
						r += 2;

						OfxRangeD range;
						range.min = min;
						range.max = max;
						rangeMap[clip].push_back( range );
					}
				}
			}
		}
	}

	return stat;
}

OfxStatus Instance::isIdentityAction( OfxTime&           time,
                                      const std::string& field,
                                      const OfxRectI&    renderRoI,
                                      OfxPointD          renderScale,
                                      std::string&       clip )
{
	static property::OfxhPropSpec inStuff[] = {
		{ kOfxPropTime, property::eDouble, 1, true, "0" },
		{ kOfxImageEffectPropFieldToRender, property::eString, 1, true, "" },
		{ kOfxImageEffectPropRenderWindow, property::eInt, 4, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	static property::OfxhPropSpec outStuff[] = {
		{ kOfxPropTime, property::eDouble, 1, false, "0.0" },
		{ kOfxPropName, property::eString, 1, false, "" },
		{ 0 }
	};

	property::OfxhSet inArgs( inStuff );

	inArgs.setStringProperty( kOfxImageEffectPropFieldToRender, field );
	inArgs.setDoubleProperty( kOfxPropTime, time );
	inArgs.setIntPropertyN( kOfxImageEffectPropRenderWindow, &renderRoI.x1, 4 );
	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	property::OfxhSet outArgs( outStuff );

	OfxStatus st = mainEntry( kOfxImageEffectActionIsIdentity,
	                          this->getHandle(),
	                          &inArgs,
	                          &outArgs );

	if( st == kOfxStatOK )
	{
		time = outArgs.getDoubleProperty( kOfxPropTime );
		clip = outArgs.getStringProperty( kOfxPropName );
	}

	return st;
}

/// Get whether the component is a supported 'chromatic' component (RGBA or alpha) in
/// the base API.
/// Override this if you have extended your chromatic colour types (eg RGB) and want
/// the clip preferences logic to still work

bool Instance::isChromaticComponent( const std::string& str ) const
{
	if( str == kOfxImageComponentRGBA )
		return true;
	if( str == kOfxImageComponentAlpha )
		return true;
	return false;
}

/// function to check for multiple bit depth support
/// The answer will depend on host, plugin and context

bool Instance::canCurrentlyHandleMultipleClipDepths() const
{
	/// does the host support 'em
	bool hostSupports = tuttle::host::core::Core::instance().getHost().getProperties().getIntProperty( kOfxImageEffectPropSupportsMultipleClipDepths ) != 0;

	/// does the plug-in support 'em
	bool pluginSupports = supportsMultipleClipDepths();

	/// no support, so no
	if( !hostSupports || !pluginSupports )
		return false;

	/// if filter context, no support, tempted to change this though
	if( _context == kOfxImageEffectContextFilter )
		return false;

	/// if filter context, no support
	if( _context == kOfxImageEffectContextGenerator ||
	    _context == kOfxImageEffectContextTransition ||
	    _context == kOfxImageEffectContextPaint ||
	    _context == kOfxImageEffectContextRetimer )
		return false;

	/// OK we're cool
	return true;
}

/// Setup the default clip preferences on the clips

void Instance::setDefaultClipPreferences()
{
	/// is there multiple bit depth support? Depends on host, plugin and context
	bool multiBitDepth = canCurrentlyHandleMultipleClipDepths();

	/// OK find the deepest chromatic component on our input clips and the one with the
	/// most components
	std::string deepestBitDepth = kOfxBitDepthNone;
	std::string mostComponents  = kOfxImageComponentNone;
	double frameRate            = 0;
	std::string premult         = kOfxImageOpaque;

	for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
	     it != _clips.end();
	     it++ )
	{
		attribute::OfxhClipImage* clip = it->second;

		// If not output clip
		if( !clip->isOutput() )
		{
			frameRate = Maximum( frameRate, clip->getFrameRate() );

			std::string rawComp = clip->getUnmappedComponents();
			rawComp = clip->findSupportedComp( rawComp ); // turn that into a comp the plugin expects on that clip

			const std::string& rawDepth   = clip->getUnmappedBitDepth();
			const std::string& rawPreMult = clip->getPremult();

			if( isChromaticComponent( rawComp ) )
			{
				if( rawPreMult == kOfxImagePreMultiplied )
					premult = kOfxImagePreMultiplied;
				else if( rawPreMult == kOfxImageUnPreMultiplied && premult != kOfxImagePreMultiplied )
					premult = kOfxImageUnPreMultiplied;
				deepestBitDepth = FindDeepestBitDepth( deepestBitDepth, rawDepth );
				mostComponents  = findMostChromaticComponents( mostComponents, rawComp );
			}
		}
	}

	/// set some stuff up
	_outputFrameRate         = frameRate;
	_outputFielding          = getDefaultOutputFielding();
	_outputPreMultiplication = premult;
	_continuousSamples       = false;
	_frameVarying            = false;

	/// now find the best depth that the plugin supports
	deepestBitDepth = bestSupportedDepth( deepestBitDepth );

	/// now add the clip gubbins to the out args
	for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		attribute::OfxhClipImage* clip = it->second;
		std::string comp, depth;

		std::string rawComp = clip->getUnmappedComponents();
		rawComp = clip->findSupportedComp( rawComp ); // turn that into a comp the plugin expects on that clip
		const std::string& rawDepth = clip->getUnmappedBitDepth();
		if( isChromaticComponent( rawComp ) )
		{

			if( clip->isOutput() )
			{
				depth = deepestBitDepth;
				comp  = clip->findSupportedComp( mostComponents );

				clip->setPixelDepth( depth );
				clip->setComponents( comp );
			}
			else
			{
				comp  = rawComp;
				depth = multiBitDepth ? bestSupportedDepth( rawDepth ) : deepestBitDepth;

				clip->setPixelDepth( depth );
				clip->setComponents( comp );
			}
		}
		else
		{
			/// hmm custom component type, don't touch it and pass it through
			clip->setPixelDepth( rawDepth );
			clip->setComponents( rawComp );
		}
	}
}

/// Initialise the clip preferences arguments, override this to do
/// stuff with wierd components etc...

void Instance::setupClipPreferencesArgs( property::OfxhSet& outArgs )
{
	/// reset all the clip prefs stuff to their defaults
	setDefaultClipPreferences();

	static property::OfxhPropSpec clipPrefsStuffs [] = {
		{ kOfxImageEffectPropFrameRate, property::eDouble, 1, false, "1" },
		{ kOfxImageEffectPropPreMultiplication, property::eString, 1, false, "" },
		{ kOfxImageClipPropFieldOrder, property::eString, 1, false, "" },
		{ kOfxImageClipPropContinuousSamples, property::eInt, 1, false, "0" },
		{ kOfxImageEffectFrameVarying, property::eInt, 1, false, "0" },
		{ 0 }
	};

	outArgs.addProperties( clipPrefsStuffs );

	/// set the default for those

	/// is there multiple bit depth support? Depends on host, plugin and context
	bool multiBitDepth = canCurrentlyHandleMultipleClipDepths();

	outArgs.setStringProperty( kOfxImageClipPropFieldOrder, _outputFielding );
	outArgs.setStringProperty( kOfxImageEffectPropPreMultiplication, _outputPreMultiplication );
	outArgs.setDoubleProperty( kOfxImageEffectPropFrameRate, _outputFrameRate );

	/// now add the clip gubbins to the out args
	for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
	     it != _clips.end();
	     it++ )
	{
		attribute::OfxhClipImage* clip = it->second;

		std::string componentParamName = "OfxImageClipPropComponents_" + it->first;
		std::string depthParamName     = "OfxImageClipPropDepth_" + it->first;
		std::string parParamName       = "OfxImageClipPropPAR_" + it->first;

		property::OfxhPropSpec specComp = { componentParamName.c_str(), property::eString, 0, false, "" }; // note the support for multi-planar clips
		outArgs.createProperty( specComp );
		// as it is variable dimension, there is no default value, so we have to set it explicitly
		outArgs.setStringProperty( componentParamName.c_str(), clip->getComponents().c_str() );
		property::OfxhPropSpec specDep = { depthParamName.c_str(), property::eString, 1, !multiBitDepth, clip->getPixelDepth().c_str() };
		outArgs.createProperty( specDep );
		outArgs.setStringProperty( depthParamName.c_str(), clip->getPixelDepth().c_str() );
		property::OfxhPropSpec specPAR = { parParamName.c_str(), property::eDouble, 1, false, "1" };
		outArgs.createProperty( specPAR );
		outArgs.setDoubleProperty( parParamName.c_str(), 1.0 ); // Default pixel aspect ratio is set to 1.0
	}
}

void Instance::setupClipInstancePreferences( property::OfxhSet& outArgs )
{

	for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
	     it != _clips.end();
	     it++ )
	{

		attribute::OfxhClipImage* clip = it->second;

		// Properties setup
		std::string componentParamName = "OfxImageClipPropComponents_" + it->first;
		std::string depthParamName     = "OfxImageClipPropDepth_" + it->first;
		std::string parParamName       = "OfxImageClipPropPAR_" + it->first;
		clip->setPixelDepth( outArgs.getStringProperty( depthParamName ) );
		clip->setComponents( outArgs.getStringProperty( componentParamName ) );
		clip->setPixelAspectRatio( outArgs.getDoubleProperty( parParamName.c_str() ) );
	}

	_outputFrameRate         = outArgs.getDoubleProperty( kOfxImageEffectPropFrameRate );
	_outputFielding          = outArgs.getStringProperty( kOfxImageClipPropFieldOrder );
	_outputPreMultiplication = outArgs.getStringProperty( kOfxImageEffectPropPreMultiplication );
	_continuousSamples       = outArgs.getIntProperty( kOfxImageClipPropContinuousSamples ) != 0;
	_frameVarying            = outArgs.getIntProperty( kOfxImageEffectFrameVarying ) != 0;
}

/// the idea here is the clip prefs live as active props on the effect
/// and are set up by clip preferences. The action manages the clip
/// preferences bits. We also monitor clip and param changes and
/// flag when clip prefs is dirty.
/// call the clip preferences action

bool Instance::getClipPreferences()
{
	/// create the out args with the stuff that does not depend on individual clips
	property::OfxhSet outArgs;

	setupClipPreferencesArgs( outArgs );

	OfxStatus st = mainEntry( kOfxImageEffectActionGetClipPreferences,
	                          this->getHandle(),
	                          0,
	                          &outArgs );

	if( st != kOfxStatOK && st != kOfxStatReplyDefault )
	{
		/// ouch
		return false;
	}

	// Setup members data from loaded properties
	setupClipInstancePreferences( outArgs );

	_clipPrefsDirty = false;

	return true;
}

/// find the most chromatic components out of the two. Override this if you define
/// more chromatic components

const std::string& Instance::findMostChromaticComponents( const std::string& a, const std::string& b ) const
{
	if( a == kOfxImageComponentNone )
		return b;
	if( a == kOfxImageComponentRGBA )
		return a;
	if( b == kOfxImageComponentRGBA )
		return b;
	return a;
}

/// given the bit depth, find the best match for it.

const std::string& Instance::bestSupportedDepth( const std::string& depth ) const
{
	static const std::string none( kOfxBitDepthNone );
	static const std::string bytes( kOfxBitDepthByte );
	static const std::string shorts( kOfxBitDepthShort );
	static const std::string floats( kOfxBitDepthFloat );

	if( depth == none )
		return none;

	if( isPixelDepthSupported( depth ) )
		return depth;

	if( depth == floats )
	{
		if( isPixelDepthSupported( shorts ) )
			return shorts;
		if( isPixelDepthSupported( bytes ) )
			return bytes;
	}

	if( depth == shorts )
	{
		if( isPixelDepthSupported( floats ) )
			return floats;
		if( isPixelDepthSupported( bytes ) )
			return bytes;
	}

	if( depth == bytes )
	{
		if( isPixelDepthSupported( shorts ) )
			return shorts;
		if( isPixelDepthSupported( floats ) )
			return floats;
	}

	/// WTF? Something wrong here
	return none;
}

OfxStatus Instance::getTimeDomainAction( OfxRangeD& range )
{
	property::OfxhPropSpec outStuff[] = {
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, false, "0.0" },
		{ 0 }
	};

	property::OfxhSet outArgs( outStuff );

	OfxStatus st = mainEntry( kOfxImageEffectActionGetTimeDomain,
	                          this->getHandle(),
	                          0,
	                          &outArgs );

	if( st != kOfxStatOK )
		return st;

	range.min = outArgs.getDoubleProperty( kOfxImageEffectActionGetTimeDomain, 0 );
	range.max = outArgs.getDoubleProperty( kOfxImageEffectActionGetTimeDomain, 1 );

	return kOfxStatOK;
}

/// implemented for Param::SetInstance

void Instance::paramChangedByPlugin( attribute::OfxhParam* param )
{
	double frame = getFrameRecursive();
	OfxPointD renderScale;

	getRenderScaleRecursive( renderScale.x, renderScale.y );

	beginInstanceChangedAction( kOfxChangePluginEdited );
	paramInstanceChangedAction( param->getName(), kOfxChangePluginEdited, frame, renderScale );
	endInstanceChangedAction( kOfxChangePluginEdited );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// The image effect suite functions

static OfxStatus getPropertySet( OfxImageEffectHandle  h1,
                                 OfxPropertySetHandle* h2 )
{
	Base* effectBase = reinterpret_cast<Base*>( h1 );

	if( !effectBase || !effectBase->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	*h2 = effectBase->getProperties().getHandle();
	return kOfxStatOK;
}

static OfxStatus getParamSet( OfxImageEffectHandle h1,
                              OfxParamSetHandle*   h2 )
{
	imageEffect::Base* effectBase = reinterpret_cast<imageEffect::Base*>( h1 );

	if( !effectBase || !effectBase->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	imageEffect::Descriptor* effectDescriptor = dynamic_cast<imageEffect::Descriptor*>( effectBase );

	if( effectDescriptor )
	{
		*h2 = effectDescriptor->getParamSetHandle();
		return kOfxStatOK;
	}

	imageEffect::Instance* effectInstance = dynamic_cast<imageEffect::Instance*>( effectBase );

	if( effectInstance )
	{
		*h2 = effectInstance->getParamSetHandle();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus clipDefine( OfxImageEffectHandle  h1,
                             const char*           name,
                             OfxPropertySetHandle* h2 )
{
	imageEffect::Base* effectBase = reinterpret_cast<imageEffect::Base*>( h1 );

	if( !effectBase || !effectBase->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	imageEffect::Descriptor* effectDescriptor = dynamic_cast<imageEffect::Descriptor*>( effectBase );

	if( effectDescriptor )
	{
		attribute::OfxhClipImageDescriptor* clip = effectDescriptor->defineClip( name );
		*h2 = clip->getPropHandle();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus clipGetPropertySet( OfxImageClipHandle    clip,
                                     OfxPropertySetHandle* propHandle )
{
	attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>( clip );

	if( !clipInstance || !clipInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	if( clipInstance )
	{
		*propHandle = clipInstance->getPropHandle();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus clipGetImage( OfxImageClipHandle    h1,
                               OfxTime               time,
                               OfxRectD*             h2,
                               OfxPropertySetHandle* h3 )
{
	attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>( h1 );

	if( !clipInstance || !clipInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	if( clipInstance )
	{
		OfxhImage* image = clipInstance->getImage( time, h2 );
		if( !image )
		{
			h3 = NULL;
			return kOfxStatFailed;
		}

		*h3 = image->getPropHandle();

		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus clipReleaseImage( OfxPropertySetHandle h1 )
{
	property::OfxhSet* pset = reinterpret_cast<property::OfxhSet*>( h1 );

	if( !pset || !pset->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhImage* image = dynamic_cast<OfxhImage*>( pset );

	if( image )
	{
		// clip::image has a virtual destructor for derived classes
		if( image->releaseReference() )
			delete image;
		return kOfxStatOK;
	}
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus clipGetHandle( OfxImageEffectHandle  imageEffect,
                                const char*           name,
                                OfxImageClipHandle*   clip,
                                OfxPropertySetHandle* propertySet )
{
	imageEffect::Base* effectBase = reinterpret_cast<imageEffect::Base*>( imageEffect );

	if( !effectBase || !effectBase->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	imageEffect::Instance* effectInstance = reinterpret_cast<imageEffect::Instance*>( effectBase );

	if( effectInstance )
	{
		attribute::OfxhClipImage& instance = effectInstance->getClip( name );
		*clip = instance.getOfxImageClipHandle();
		if( propertySet )
			*propertySet = instance.getPropHandle();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus clipGetRegionOfDefinition( OfxImageClipHandle clip,
                                            OfxTime            time,
                                            OfxRectD*          bounds )
{
	attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>( clip );

	if( !clipInstance || !clipInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	if( clipInstance )
	{
		*bounds = clipInstance->getRegionOfDefinition( time );
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

// should processing be aborted?

static int abort( OfxImageEffectHandle imageEffect )
{
	imageEffect::Base* effectBase = reinterpret_cast<imageEffect::Base*>( imageEffect );

	if( !effectBase || !effectBase->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	imageEffect::Instance* effectInstance = dynamic_cast<imageEffect::Instance*>( effectBase );

	if( effectInstance )
		return effectInstance->abort();
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus imageMemoryAlloc( OfxImageEffectHandle  instanceHandle,
                                   size_t                nBytes,
                                   OfxImageMemoryHandle* memoryHandle )
{
	imageEffect::Base* effectBase         = reinterpret_cast<imageEffect::Base*>( instanceHandle );
	imageEffect::Instance* effectInstance = reinterpret_cast<imageEffect::Instance*>( effectBase );
	memory::OfxhMemory* memory;

	if( effectInstance )
	{

		if( !effectInstance->verifyMagic() )
		{
			return kOfxStatErrBadHandle;
		}

		memory = effectInstance->imageMemoryAlloc( nBytes );
	}
	else
	{
		memory = new memory::OfxhMemory;
		memory->alloc( nBytes );
	}

	*memoryHandle = memory->getHandle();
	return kOfxStatOK;
}

static OfxStatus imageMemoryFree( OfxImageMemoryHandle memoryHandle )
{
	memory::OfxhMemory* memoryInstance = reinterpret_cast<memory::OfxhMemory*>( memoryHandle );

	if( memoryInstance && memoryInstance->verifyMagic() )
	{
		memoryInstance->freeMem();
		delete memoryInstance;
		return kOfxStatOK;
	}
	else
		return kOfxStatErrBadHandle;
}

OfxStatus imageMemoryLock( OfxImageMemoryHandle memoryHandle,
                           void**               returnedPtr )
{
	memory::OfxhMemory* memoryInstance = reinterpret_cast<memory::OfxhMemory*>( memoryHandle );

	if( memoryInstance && memoryInstance->verifyMagic() )
	{
		memoryInstance->lock();
		*returnedPtr = memoryInstance->getPtr();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus imageMemoryUnlock( OfxImageMemoryHandle memoryHandle )
{
	memory::OfxhMemory* memoryInstance = reinterpret_cast<memory::OfxhMemory*>( memoryHandle );

	if( memoryInstance && memoryInstance->verifyMagic() )
	{
		memoryInstance->unlock();
		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static struct OfxImageEffectSuiteV1 gImageEffectSuite =
{
	getPropertySet,
	getParamSet,
	clipDefine,
	clipGetHandle,
	clipGetPropertySet,
	clipGetImage,
	clipReleaseImage,
	clipGetRegionOfDefinition,
	abort,
	imageMemoryAlloc,
	imageMemoryFree,
	imageMemoryLock,
	imageMemoryUnlock
};

/// message suite function for an image effect

static OfxStatus message( void* handle, const char* type, const char* id, const char* format, ... )
{
	imageEffect::Instance* effectInstance = reinterpret_cast<imageEffect::Instance*>( handle );

	if( effectInstance )
	{
		va_list args;
		va_start( args, format );
		effectInstance->vmessage( type, id, format, args );
		va_end( args );
	}
	else
	{
		va_list args;
		va_start( args, format );
		vprintf( format, args );
		va_end( args );
	}
	return kOfxStatOK;
}

/// message suite for an image effect plugin
static struct OfxMessageSuiteV1 gMessageSuite =
{
	message
};

////////////////////////////////////////////////////////////////////////////////
/// make an overlay interact for an image effect

OverlayInteract::OverlayInteract( imageEffect::Instance& effect )
	: interact::OfxhInteract( effect.getOverlayDescriptor(), ( void* )( effect.getHandle() ) ),
	_instance( effect )
{}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Progress suite functions

/// begin progressing

static OfxStatus ProgressStart( void*       effectInstance,
                                const char* label )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );

	me->progressStart( label );
	return kOfxStatOK;
}

/// finish progressing

static OfxStatus ProgressEnd( void* effectInstance )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );

	me->progressEnd();
	return kOfxStatOK;
}

/// update progressing

static OfxStatus ProgressUpdate( void* effectInstance, double progress )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );
	bool v       = me->progressUpdate( progress );

	return v ? kOfxStatOK : kOfxStatReplyNo;
}

/// our progress suite
struct OfxProgressSuiteV1 gProgressSuite =
{
	ProgressStart,
	ProgressUpdate,
	ProgressEnd
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// timeline suite functions

/// timeline suite function

static OfxStatus TimeLineGetTime( void* effectInstance, double* time )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );

	*time = me->timeLineGetTime();
	return kOfxStatOK;
}

/// timeline suite function

static OfxStatus TimeLineGotoTime( void* effectInstance, double time )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );

	me->timeLineGotoTime( time );
	return kOfxStatOK;
}

/// timeline suite function

static OfxStatus TimeLineGetBounds( void* effectInstance, double* firstTime, double* lastTime )
{
	Instance* me = reinterpret_cast<Instance*>( effectInstance );

	me->timeLineGetBounds( *firstTime, *lastTime );
	return kOfxStatOK;
}

/// our progress suite
struct OfxTimeLineSuiteV1 gTimelineSuite =
{
	TimeLineGetTime,
	TimeLineGotoTime,
	TimeLineGetBounds
};

////////////////////////////////////////////////////////////////////////////////
/// a simple multithread suite

static OfxStatus multiThread( OfxThreadFunctionV1 func,
                              unsigned int        nThreads,
                              void*               customArg )
{
	func( 0, 1, customArg );
	return kOfxStatOK;
}

static OfxStatus multiThreadNumCPUs( unsigned int* nCPUs )
{
	*nCPUs = 1;
	return kOfxStatOK;
}

static OfxStatus multiThreadIndex( unsigned int* threadIndex )
{
	threadIndex = 0;
	return kOfxStatOK;
}

static int multiThreadIsSpawnedThread( void )
{
	return false;
}

static OfxStatus mutexCreate( const OfxMutexHandle* mutex, int lockCount )
{
	// do nothing single threaded
	mutex = 0;
	return kOfxStatOK;
}

static OfxStatus mutexDestroy( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexUnLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexTryLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static struct OfxMultiThreadSuiteV1 gSingleThreadedSuite =
{
	multiThread,
	multiThreadNumCPUs,
	multiThreadIndex,
	multiThreadIsSpawnedThread,
	mutexCreate,
	mutexDestroy,
	mutexLock,
	mutexUnLock,
	mutexTryLock
};

////////////////////////////////////////////////////////////////////////////////
/// The image effect host

/// properties for the image effect host
static property::OfxhPropSpec hostStuffs[] = {
	{ kOfxImageEffectHostPropIsBackground, property::eInt, 1, true, "0" },
	{ kOfxImageEffectPropSupportsOverlays, property::eInt, 1, true, "1" },
	{ kOfxImageEffectPropSupportsMultiResolution, property::eInt, 1, true, "1" },
	{ kOfxImageEffectPropSupportsTiles, property::eInt, 1, true, "1" },
	{ kOfxImageEffectPropTemporalClipAccess, property::eInt, 1, true, "1" },

	/// xxx this needs defaulting manually
	{ kOfxImageEffectPropSupportedComponents, property::eString, 0, true, "" },
	/// xxx this needs defaulting manually

	{ kOfxImageEffectPropSupportedPixelDepths, property::eString, 0, true, "" },

	/// xxx this needs defaulting manually

	{ kOfxImageEffectPropSupportedContexts, property::eString, 0, true, "" },
	/// xxx this needs defaulting manually

	{ kOfxImageEffectPropSupportsMultipleClipDepths, property::eInt, 1, true, "1" },
	{ kOfxImageEffectPropSupportsMultipleClipPARs, property::eInt, 1, true, "0" },
	{ kOfxImageEffectPropSetableFrameRate, property::eInt, 1, true, "0" },
	{ kOfxImageEffectPropSetableFielding, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropSupportsCustomInteract, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropSupportsStringAnimation, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropSupportsChoiceAnimation, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropSupportsBooleanAnimation, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropSupportsCustomAnimation, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropMaxParameters, property::eInt, 1, true, "-1" },
	{ kOfxParamHostPropMaxPages, property::eInt, 1, true, "0" },
	{ kOfxParamHostPropPageRowColumnCount, property::eInt, 2, true, "0" },
	{ 0 },
};

/// ctor

ImageEffectHost::ImageEffectHost()
{
	/// add the properties for an image effect host, derived classs to set most of them
	_properties.addProperties( hostStuffs );
}

ImageEffectHost::~ImageEffectHost()
{}

/**
 * optionally over-ridden function to register the creation of a new descriptor in the host app
 */
void ImageEffectHost::initDescriptor( Descriptor* desc ) const {}

/**
 * Use this in any dialogue etc... showing progress
 */
void ImageEffectHost::loadingStatus( const std::string& ) {}

bool ImageEffectHost::pluginSupported( OfxhImageEffectPlugin* plugin, std::string& reason ) const
{
	return true;
}

/**
 * our suite fetcher
 */
void* ImageEffectHost::fetchSuite( const char* suiteName, int suiteVersion )
{
	if( strcmp( suiteName, kOfxImageEffectSuite ) == 0 )
	{
		if( suiteVersion == 1 )
			return (void*) &gImageEffectSuite;
		else
			return NULL;
	}
	//else TUTTLE_TODO : move to ofx::host::Host
	else if( strcmp( suiteName, kOfxParameterSuite ) == 0 )
	{
		return attribute::GetSuite( suiteVersion );
	}
	else if( strcmp( suiteName, kOfxMessageSuite ) == 0 )
	{
		if( suiteVersion == 1 )
			return (void*) &gMessageSuite;
		else
			return NULL;
	}
	else if( strcmp( suiteName, kOfxInteractSuite ) == 0 )
	{
		return interact::GetSuite( suiteVersion );
	}
	else if( strcmp( suiteName, kOfxProgressSuite ) == 0 )
	{
		if( suiteVersion == 1 )
			return (void*) &gProgressSuite;
		else
			return NULL;
	}
	else if( strcmp( suiteName, kOfxTimeLineSuite ) == 0 )
	{
		if( suiteVersion == 1 )
			return (void*) &gTimelineSuite;
		else
			return NULL;
	}
	else if( strcmp( suiteName, kOfxMultiThreadSuite ) == 0 )
	{
		if( suiteVersion == 1 )
			return (void*) &gSingleThreadedSuite;
		else
			return NULL;
	}
	else /// otherwise just grab the base class one, which is props and memory
		return tuttle::host::ofx::OfxhAbstractHost::fetchSuite( suiteName, suiteVersion );
}

}
}
}
}
