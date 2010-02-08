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
#include "OfxhProperty.hpp"
#include "OfxhClip.hpp"
#include "OfxhParam.hpp"
#include "OfxhMemory.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"
#include "OfxhImageEffectPlugin.hpp"
#include "OfxhUtilities.hpp"

#include <tuttle/host/core/Core.hpp>

#include <cstring>
#include <cstdarg>
#include <cmath>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {


static const property::OfxhPropSpec effectInstanceStuff[] = {
	/* name                                 type                   dim.   r/o    default value */
	{ kOfxPropType, property::eString, 1, true, kOfxTypeImageEffectInstance },
	{ kOfxPropName, property::eString, 1, false, "UNIQUE_NAME_NOT_SET" },
	{ kOfxImageEffectPropContext, property::eString, 1, true, "" },
	{ kOfxPropInstanceData, property::ePointer, 1, false, NULL },
	{ kOfxImageEffectPropPluginHandle, property::ePointer, 1, false, NULL },
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

OfxhImageEffectNode::OfxhImageEffectNode( const OfxhImageEffectPlugin*         plugin,
                                          const OfxhImageEffectNodeDescriptor& descriptor,
                                          const std::string&                   context,
                                          bool                                 interactive )
	: OfxhImageEffectNodeBase( effectInstanceStuff ),
	_plugin( plugin ),
	_context( context ),
	_descriptor( descriptor ),
	_interactive( interactive ),
	_created( false ),
	_continuousSamples( false ),
	_frameVarying( false ),
	_outputFrameRate( 24 )
{
	_properties.setChainedSet( &descriptor.getProperties() );

	_properties.setPointerProperty( kOfxImageEffectPropPluginHandle, const_cast<OfxPlugin*>( _plugin->getPluginHandle()->getOfxPlugin() ) );

	_properties.setStringProperty( kOfxImageEffectPropContext, context );
	_properties.setIntProperty( kOfxPropIsInteractive, interactive );

	// copy is sequential over
	bool sequential = descriptor.getProperties().getIntProperty( kOfxImageEffectInstancePropSequentialRender ) != 0;
	_properties.setIntProperty( kOfxImageEffectInstancePropSequentialRender, sequential );
	
	initHook();
}

void OfxhImageEffectNode::initHook()
{
	int i = 0;
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

OfxhImageEffectNode::OfxhImageEffectNode( const OfxhImageEffectNode& other )
: OfxhImageEffectNodeBase( other.getProperties() )
//, attribute::OfxhParamSet( other )
//, attribute::OfxhClipImageSet( other )
, _plugin( other.getPlugin() )
, _context( other.getContext() )
, _descriptor( other.getDescriptor() )
, _interactive( other._interactive )
, _created( false )
, _continuousSamples( other._continuousSamples )
, _frameVarying( other._frameVarying )
, _outputFrameRate( other._outputFrameRate )
{
	_properties.setChainedSet( &_descriptor.getProperties() );
	initHook();
}

/**
 * called after construction to populate clips and params
 */
void OfxhImageEffectNode::populate()
{
	populateClips( _descriptor );
	populateParams( _descriptor );
}

/**
 * @todo tuttle: move this in ParamSet !
 */
void OfxhImageEffectNode::populateParams( const imageEffect::OfxhImageEffectNodeDescriptor& descriptor ) throw( core::exception::LogicError )
{
	const attribute::OfxhParamDescriptorSet::ParamDescriptorList& paramDescriptors = _descriptor.getParamList();

	std::map<std::string, attribute::OfxhParam*> parameters;

	// Create parameters on their own groups
	for( attribute::OfxhParamDescriptorSet::ParamDescriptorList::const_iterator it = paramDescriptors.begin(), itEnd = paramDescriptors.end();
	     it != itEnd;
	     ++it )
	{
		attribute::OfxhParamSet* setInstance = this;
		// SetInstance where the childrens param instances will be added
		const attribute::OfxhParamDescriptor& descriptor = *it;

		// name and parentName of the parameter
		std::string name       = descriptor.getName();
		std::string parentName = descriptor.getParentName();

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
		attribute::OfxhParam* instance = newParam( descriptor );
		/// @todo tuttle set the groups of the ParamInstance !!!
		parameters[name] = instance;

	}

}

/**
 * @todo tuttle check clip ? check hash ? etc.
 */
bool OfxhImageEffectNode::operator==( const OfxhImageEffectNode& other ) const
{
	if( OfxhImageEffectNodeBase::operator!=( other ) )
		return false;
	if( attribute::OfxhParamSet::operator!=( other ) )
		return false;
	if( attribute::OfxhClipImageSet::operator!=( other ) )
		return false;
	return true;
}

/**
 * implemented for Param::SetDescriptor
 */
property::OfxhSet& OfxhImageEffectNode::getParamSetProps()
{
	return _properties;
}

// do nothing
size_t OfxhImageEffectNode::getDimension( const std::string& name ) const OFX_EXCEPTION_SPEC
{
	COUT_ERROR( "failing in " << __PRETTY_FUNCTION__ << " with name=" << name );
	throw OfxhException( kOfxStatErrMissingHostFeature );
}

size_t OfxhImageEffectNode::upperGetDimension( const std::string& name )
{
	return _properties.getDimension( name );
}

void OfxhImageEffectNode::notify( const std::string& name, bool singleValue, int indexOrN ) OFX_EXCEPTION_SPEC
{
	COUT_ERROR( "failing in " << __PRETTY_FUNCTION__ );
}

/**
 * don't know what to do
 */
void OfxhImageEffectNode::reset( const std::string& name ) OFX_EXCEPTION_SPEC
{
	COUT_ERROR( "failing in " << __PRETTY_FUNCTION__ );
	throw OfxhException( kOfxStatErrMissingHostFeature );
}

/**
 * get the virutals for viewport size, pixel scale, background colour
 */
double OfxhImageEffectNode::getDoubleProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxImageEffectPropProjectSize )
	{
		if( index >= 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectSize( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectOffset )
	{
		if( index >= 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectOffset( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectExtent )
	{
		if( index >= 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		double values[2];
		getProjectExtent( values[0], values[1] );
		return values[index];
	}
	else if( name == kOfxImageEffectPropProjectPixelAspectRatio )
	{
		if( index >= 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		return getProjectPixelAspectRatio();
	}
	else if( name == kOfxImageEffectInstancePropEffectDuration )
	{
		if( index >= 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		return getEffectDuration();
	}
	else if( name == kOfxImageEffectPropFrameRate )
	{
		if( index >= 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		return getFrameRate();
	}
	else
		throw OfxhException( kOfxStatErrUnknown );
}

void OfxhImageEffectNode::getDoublePropertyN( const std::string& name, double* first, int n ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxImageEffectPropProjectSize )
	{
		if( n > 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		getProjectSize( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectOffset )
	{
		if( n > 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		getProjectOffset( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectExtent )
	{
		if( n > 2 )
			throw OfxhException( kOfxStatErrBadIndex );
		getProjectExtent( first[0], first[1] );
	}
	else if( name == kOfxImageEffectPropProjectPixelAspectRatio )
	{
		if( n > 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		*first = getProjectPixelAspectRatio();
	}
	else if( name == kOfxImageEffectInstancePropEffectDuration )
	{
		if( n > 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		*first = getEffectDuration();
	}
	else if( name == kOfxImageEffectPropFrameRate )
	{
		if( n > 1 )
			throw OfxhException( kOfxStatErrBadIndex );
		*first = getFrameRate();
	}
	else
		throw OfxhException( kOfxStatErrUnknown );
}

OfxhImageEffectNode::~OfxhImageEffectNode()
{
	// destroy the instance, only if succesfully created
	if( _created )
	{
		mainEntry( kOfxActionDestroyInstance, this->getHandle(), 0, 0 );
	}
}

/**
 * this is used to populate with any extra action in arguments that may be needed
 */
void OfxhImageEffectNode::setCustomInArgs( const std::string& action, property::OfxhSet& inArgs ) {}

/**
 * this is used to populate with any extra action out arguments that may be needed
 */
void OfxhImageEffectNode::setCustomOutArgs( const std::string& action, property::OfxhSet& outArgs ) {}

/**
 * this is used to populate with any extra action out arguments that may be needed
 */
void OfxhImageEffectNode::examineOutArgs( const std::string& action, OfxStatus, const property::OfxhSet& outArgs ) {}

/**
 * check for connection
 */
bool OfxhImageEffectNode::checkClipConnectionStatus() const
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

/**
 * override this to make processing abort, return 1 to abort processing
 */
int OfxhImageEffectNode::abort()
{
	return 0;
}

/**
 * override this to use your own memory instance - must inherrit from memory::instance
 */
OfxhMemory* OfxhImageEffectNode::newMemoryInstance( size_t nBytes )
{
	return 0;
}

/**
 * @return an memory::instance calls makeMemoryInstance that can be overriden
 */
OfxhMemory* OfxhImageEffectNode::imageMemoryAlloc( size_t nBytes )
{
	OfxhMemory* instance = newMemoryInstance( nBytes );

	if( instance )
		return instance;
	else
	{
		OfxhMemory* instance = new OfxhMemory();
		instance->alloc( nBytes );
		return instance;
	}
}

/**
 * call the effect entry point
 */
OfxStatus OfxhImageEffectNode::mainEntry( const char*        action,
                                          const void*        handle,
                                          property::OfxhSet* inArgs,
                                          property::OfxhSet* outArgs )
{
	if( !_plugin )
		return kOfxStatFailed;
	
	const OfxhPluginHandle* pHandle = _plugin->getPluginHandle();
	if( !pHandle )
		return kOfxStatFailed;

	const OfxPlugin* ofxPlugin = pHandle->getOfxPlugin();
	if( !ofxPlugin )
		return kOfxStatFailed;

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

/**
 * create a clip instance
 */
void OfxhImageEffectNode::createInstanceAction() OFX_EXCEPTION_SPEC
{
	/// we need to init the clips before we call create instance incase
	/// they try and fetch something in create instance, which they are allowed
	setDefaultClipPreferences();

	// now tell the plug-in to create instance
	OfxStatus status = mainEntry( kOfxActionCreateInstance, this->getHandle(), 0, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status, "Create action failed on plugin " + getName() );

	_created = true;
}

/**
 * begin/change/end instance changed
 */
void OfxhImageEffectNode::beginInstanceChangedAction( const std::string& why ) OFX_EXCEPTION_SPEC
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	OfxStatus status = mainEntry( kOfxActionBeginInstanceChanged, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

void OfxhImageEffectNode::paramInstanceChangedAction( const std::string& paramName,
                                                           const std::string& why,
                                                           OfxTime            time,
                                                           OfxPointD          renderScale ) OFX_EXCEPTION_SPEC
{
	/*attribute::OfxhParam& param = */ getParam( paramName );

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

	OfxStatus status = mainEntry( kOfxActionInstanceChanged, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

void OfxhImageEffectNode::clipInstanceChangedAction( const std::string& clipName,
                                                          const std::string& why,
                                                          OfxTime            time,
                                                          OfxPointD          renderScale ) OFX_EXCEPTION_SPEC
{
	_clipPrefsDirty = true;
	std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.find( clipName );
	if( it == _clips.end() )
		throw OfxhException( kOfxStatFailed );

	it->second->instanceChangedAction( why, time, renderScale );
}

void OfxhImageEffectNode::endInstanceChangedAction( const std::string& why ) OFX_EXCEPTION_SPEC
{
	property::OfxhPropSpec whyStuff[] = {
		{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
		{ 0 }
	};

	property::OfxhSet inArgs( whyStuff );

	OfxStatus status = mainEntry( kOfxActionEndInstanceChanged, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

/**
 * purge plugin caches
 */
void OfxhImageEffectNode::purgeCachesAction() OFX_EXCEPTION_SPEC
{
	OfxStatus status = mainEntry( kOfxActionPurgeCaches, this->getHandle(), 0, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

/**
 * sync plugin private data
 */
void OfxhImageEffectNode::syncPrivateDataAction() OFX_EXCEPTION_SPEC
{
	OfxStatus status = mainEntry( kOfxActionSyncPrivateData, this->getHandle(), 0, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

/**
 * end edit instance
 */
void OfxhImageEffectNode::beginInstanceEditAction() OFX_EXCEPTION_SPEC
{
	OfxStatus status = mainEntry( kOfxActionBeginInstanceEdit, this->getHandle(), 0, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

/**
 * end edit instance
 */
void OfxhImageEffectNode::endInstanceEditAction() OFX_EXCEPTION_SPEC
{
	OfxStatus status = mainEntry( kOfxActionEndInstanceEdit, this->getHandle(), 0, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

void OfxhImageEffectNode::beginRenderAction( OfxTime   startFrame,
                                                  OfxTime   endFrame,
                                                  OfxTime   step,
                                                  bool      interactive,
                                                  OfxPointD renderScale ) OFX_EXCEPTION_SPEC
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

	OfxStatus status = mainEntry( kOfxImageEffectActionBeginSequenceRender, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

void OfxhImageEffectNode::renderAction( OfxTime            time,
                                             const std::string& field,
                                             const OfxRectI&    renderRoI,
                                             OfxPointD          renderScale ) OFX_EXCEPTION_SPEC
{
	static const property::OfxhPropSpec stuff[] = {
		{ kOfxPropTime, property::eDouble, 1, true, "0" },
		{ kOfxImageEffectPropFieldToRender, property::eString, 1, true, "" },
		{ kOfxImageEffectPropRenderWindow, property::eInt, 4, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	inArgs.setDoubleProperty( kOfxPropTime, time );
	inArgs.setStringProperty( kOfxImageEffectPropFieldToRender, field );
	inArgs.setIntPropertyN( kOfxImageEffectPropRenderWindow, &renderRoI.x1, 4 );
	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	//TCOUT("OfxhImageEffect::renderAction inArgs=");
	//inArgs.coutProperties();

	OfxStatus status = mainEntry( kOfxImageEffectActionRender, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

void OfxhImageEffectNode::endRenderAction( OfxTime   startFrame,
                                                OfxTime   endFrame,
                                                OfxTime   step,
                                                bool      interactive,
                                                OfxPointD renderScale ) OFX_EXCEPTION_SPEC
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

	OfxStatus status = mainEntry( kOfxImageEffectActionEndSequenceRender, this->getHandle(), &inArgs, 0 );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );
}

/**
 * calculate the default rod for this effect instance
 */
OfxRectD OfxhImageEffectNode::calcDefaultRegionOfDefinition( OfxTime   time,
                                                             OfxPointD renderScale )
{
	OfxRectD rod;

	// figure out the default contexts
	if( _context == kOfxImageEffectContextGenerator )
	{
		// generator is the extent
		rod.x1 = rod.y1 = 0;
		getProjectExtent( rod.x2, rod.y2 );
		attribute::OfxhClipImage& clip = getClip( kOfxImageEffectOutputClipName );
		rod = clip.fetchRegionOfDefinition( time );
		/// @todo tuttle: maybe RoD problems with Generator and Read here... to check !
	}
	else if( _context == kOfxImageEffectContextFilter ||
	         _context == kOfxImageEffectContextPaint )
	{
		try
		{
			// filter and paint default to the input clip
			attribute::OfxhClipImage& clip = getClip( kOfxImageEffectSimpleSourceClipName );
			rod = clip.fetchRegionOfDefinition( time );
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
			rod = clipFrom.fetchRegionOfDefinition( time );
			rod = rectUnion( rod, clipTo.fetchRegionOfDefinition( time ) );
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
					rod = clip->fetchRegionOfDefinition( time );
				else
					rod = rectUnion( rod, clip->fetchRegionOfDefinition( time ) );
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
			attribute::OfxhClipImage& clip = getClip( kOfxImageEffectSimpleSourceClipName );
			/*attribute::ParamDoubleInstance& param = */ dynamic_cast<attribute::OfxhParamDouble&>( getParam( kOfxImageEffectRetimerParamName ) );
			rod = clip.fetchRegionOfDefinition( floor( time ) );
			rod = rectUnion( rod, clip.fetchRegionOfDefinition( floor( time ) + 1 ) );
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

/**
 * RoD call
 */
void OfxhImageEffectNode::getRegionOfDefinitionAction( OfxTime   time,
                                                            OfxPointD renderScale,
                                                            OfxRectD& rod ) OFX_EXCEPTION_SPEC
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
	else
	{
		throw OfxhException( stat, "getRegionOfDefinitionAction error." );
	}
}

/**
 * get the region of interest for each input and return it in the given std::map
 */
void OfxhImageEffectNode::getRegionOfInterestAction( OfxTime time,
                                                          OfxPointD renderScale,
                                                          const OfxRectD& roi,
                                                          std::map<attribute::OfxhClipImage*, OfxRectD>& rois ) OFX_EXCEPTION_SPEC
{
	// reset the map
	rois.clear();

	if( !supportsTiles() )
	{
		/// No tiling support on the effect at all. So set the roi of each input clip to be the RoD of that clip.
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin(), itEnd = _clips.end();
		     it != itEnd;
		     ++it )
		{
			if( !it->second->isOutput() || getContext() == kOfxImageEffectContextGenerator )
			{
				/// @todo tuttle: how to support size on generators... check if this is correct in all cases.
				OfxRectD roi = it->second->fetchRegionOfDefinition( time );
				rois[it->second] = roi;
			}
		}
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
		OfxStatus status = mainEntry( kOfxImageEffectActionGetRegionsOfInterest,
		                  this->getHandle(),
		                  &inArgs,
		                  &outArgs );

		if( status != kOfxStatOK && status != kOfxStatReplyDefault )
			throw OfxhException( status );

		/// set the thing up
		for( std::map<std::string, attribute::OfxhClipImage*>::iterator it = _clips.begin();
		     it != _clips.end();
		     it++ )
		{
			if( !it->second->isOutput() || getContext() == kOfxImageEffectContextGenerator )
			{
				OfxRectD rod = it->second->fetchRegionOfDefinition( time );
				if( it->second->supportsTiles() )
				{
					std::string name = "OfxImageClipPropRoI_" + it->first;
					OfxRectD thisRoi;
					thisRoi.x1 = outArgs.getDoubleProperty( name, 0 );
					thisRoi.y1 = outArgs.getDoubleProperty( name, 1 );
					thisRoi.x2 = outArgs.getDoubleProperty( name, 2 );
					thisRoi.y2 = outArgs.getDoubleProperty( name, 3 );

					/// and clamp it to the clip's rod
					thisRoi          = clamp( thisRoi, rod );
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
}

/**
 * see how many frames are needed from each clip to render the indicated frame
 */
void OfxhImageEffectNode::getFrameNeededAction( OfxTime   time,
                                                     RangeMap& rangeMap ) OFX_EXCEPTION_SPEC
{
	OfxStatus status = kOfxStatReplyDefault;
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

		status = mainEntry( kOfxImageEffectActionGetFramesNeeded,
		                  this->getHandle(),
		                  &inArgs,
		                  &outArgs );

		if( status != kOfxStatOK && status != kOfxStatReplyDefault )
			throw OfxhException( status );
	}

	OfxRangeD defaultRange;
	defaultRange.min = defaultRange.max = time;

	for( ClipImageMap::iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		attribute::OfxhClipImage* clip = it->second;

		if( !clip->isOutput() )
		{
			if( status == kOfxStatReplyDefault )
			{
				rangeMap[clip].push_back( defaultRange );
			}
			else
			{
				std::string name = "OfxImageClipPropFrameRange_" + it->first;

				int nRanges = outArgs.getDimension( name );
				if( nRanges % 2 != 0 )
					throw OfxhException( kOfxStatErrValue ); // bad! needs to be divisible by 2

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
}

void OfxhImageEffectNode::isIdentityAction( OfxTime&           time,
                                                 const std::string& field,
                                                 const OfxRectI&    renderRoI,
                                                 OfxPointD          renderScale,
                                                 std::string&       clip ) OFX_EXCEPTION_SPEC
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

	OfxStatus status = mainEntry( kOfxImageEffectActionIsIdentity,
	                          this->getHandle(),
	                          &inArgs,
	                          &outArgs );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );

	time = outArgs.getDoubleProperty( kOfxPropTime );
	clip = outArgs.getStringProperty( kOfxPropName );
}

/**
 * Get whether the component is a supported 'chromatic' component (RGBA or alpha) in
 * the base API.
 * Override this if you have extended your chromatic colour types (eg RGB) and want
 * the clip preferences logic to still work
 */
bool OfxhImageEffectNode::isChromaticComponent( const std::string& str ) const
{
	if( str == kOfxImageComponentRGBA )
		return true;
	if( str == kOfxImageComponentAlpha )
		return true;
	return false;
}

/**
 * function to check for multiple bit depth support
 * The answer will depend on host, plugin and context
 */
bool OfxhImageEffectNode::canCurrentlyHandleMultipleClipDepths() const
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

/**
 * Setup the default clip preferences on the clips
 */
void OfxhImageEffectNode::setDefaultClipPreferences()
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
			frameRate = maximum( frameRate, clip->getFrameRate() );

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
				deepestBitDepth = findDeepestBitDepth( deepestBitDepth, rawDepth );
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

/**
 * Initialise the clip preferences arguments, override this to do
 * stuff with wierd components etc...
 */
void OfxhImageEffectNode::setupClipPreferencesArgs( property::OfxhSet& outArgs )
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

void OfxhImageEffectNode::setupClipInstancePreferences( property::OfxhSet& outArgs )
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

/**
 * the idea here is the clip prefs live as active props on the effect
 * and are set up by clip preferences. The action manages the clip
 * preferences bits. We also monitor clip and param changes and
 * flag when clip prefs is dirty.
 * call the clip preferences action
 */
void OfxhImageEffectNode::getClipPreferences() OFX_EXCEPTION_SPEC
{
	/// create the out args with the stuff that does not depend on individual clips
	property::OfxhSet outArgs;

	setupClipPreferencesArgs( outArgs );

	OfxStatus status = mainEntry( kOfxImageEffectActionGetClipPreferences,
	                          this->getHandle(),
	                          0,
	                          &outArgs );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );

	// Setup members data from loaded properties
	setupClipInstancePreferences( outArgs );

	_clipPrefsDirty = false;
}

/**
 * find the most chromatic components out of the two. Override this if you define
 * more chromatic components
 */
const std::string& OfxhImageEffectNode::findMostChromaticComponents( const std::string& a, const std::string& b ) const
{
	if( a == kOfxImageComponentNone )
		return b;
	if( a == kOfxImageComponentRGBA )
		return a;
	if( b == kOfxImageComponentRGBA )
		return b;
	return a;
}

/**
 * given the bit depth, find the best match for it.
 */
const std::string& OfxhImageEffectNode::bestSupportedDepth( const std::string& depth ) const
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

void OfxhImageEffectNode::getTimeDomainAction( OfxRangeD& range ) OFX_EXCEPTION_SPEC
{
	property::OfxhPropSpec outStuff[] = {
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, false, "0.0" },
		{ 0 }
	};

	property::OfxhSet outArgs( outStuff );

	OfxStatus status = mainEntry( kOfxImageEffectActionGetTimeDomain,
	                          this->getHandle(),
	                          0,
	                          &outArgs );

	if( status != kOfxStatOK && status != kOfxStatReplyDefault )
		throw OfxhException( status );

	range.min = outArgs.getDoubleProperty( kOfxImageEffectActionGetTimeDomain, 0 );
	range.max = outArgs.getDoubleProperty( kOfxImageEffectActionGetTimeDomain, 1 );
}

/**
 * implemented for Param::SetInstance
 */
void OfxhImageEffectNode::paramChangedByPlugin( attribute::OfxhParam* param )
{
	double frame = getFrameRecursive();
	OfxPointD renderScale;

	getRenderScaleRecursive( renderScale.x, renderScale.y );

	beginInstanceChangedAction( kOfxChangePluginEdited );
	paramInstanceChangedAction( param->getName(), kOfxChangePluginEdited, frame, renderScale );
	endInstanceChangedAction( kOfxChangePluginEdited );
}

}
}
}
}
