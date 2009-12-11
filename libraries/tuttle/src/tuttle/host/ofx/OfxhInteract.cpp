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
#include "ofxKeySyms.h"
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhPropertySuite.hpp"
#include "OfxhClip.hpp"
#include "OfxhParam.hpp"
#include "OfxhMemory.hpp"
#include "OfxhImageEffect.hpp"
#include "OfxhInteract.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace interact {

//
// descriptor
//
static property::OfxhPropSpec interactDescriptorStuffs[] = {
	{ kOfxInteractPropHasAlpha, property::eInt, 1, true, "0" },
	{ kOfxInteractPropBitDepth, property::eInt, 1, true, "0" },
	{ 0 },
};

OfxhInteractDescriptor::OfxhInteractDescriptor()
	: _properties( interactDescriptorStuffs ),
	_state( eUninitialised ),
	_entryPoint( NULL )
{}

OfxhInteractDescriptor::~OfxhInteractDescriptor()
{}

/// call describe on this descriptor
bool OfxhInteractDescriptor::describe( int bitDepthPerComponent, bool hasAlpha )
{
	if( _state == eUninitialised )
	{
		_properties.setIntProperty( kOfxInteractPropBitDepth, bitDepthPerComponent );
		_properties.setIntProperty( kOfxInteractPropHasAlpha, (int)( hasAlpha ) );

		OfxStatus stat = callEntry( kOfxActionDescribe, getHandle(), NULL, NULL );
		if( stat == kOfxStatOK || stat == kOfxStatReplyDefault )
		{
			_state = eDescribed;
		}
		else
		{
			_state = eFailed;
		}
	}
	return _state == eDescribed;
}

// call the interactive entry point
OfxStatus OfxhInteractDescriptor::callEntry( const char*          action,
                                 void*                handle,
                                 OfxPropertySetHandle inArgs,
                                 OfxPropertySetHandle outArgs ) const
{
	if( _entryPoint && _state != eFailed )
	{
		return _entryPoint( action, handle, inArgs, outArgs );
	}
	else
		return kOfxStatFailed;

	return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
static property::OfxhPropSpec interactInstanceStuffs[] = {
	{ kOfxPropEffectInstance, property::ePointer, 1, true, NULL },
	{ kOfxPropInstanceData, property::ePointer, 1, false, NULL },
	{ kOfxInteractPropPixelScale, property::eDouble, 2, true, "1.0f" },
	{ kOfxInteractPropBackgroundColour, property::eDouble, 3, true, "0.0f" },
	{ kOfxInteractPropViewportSize, property::eDouble, 2, true, "100.0f" },
	{ kOfxInteractPropSlaveToParam, property::eString, 0, false, "" },
	{ 0 },
};

static property::OfxhPropSpec interactArgsStuffs[] = {
	{ kOfxPropEffectInstance, property::ePointer, 1, false, NULL },
	{ kOfxPropTime, property::eDouble, 1, false, "0.0" },
	{ kOfxImageEffectPropRenderScale, property::eDouble, 2, false, "0.0" },
	{ kOfxInteractPropBackgroundColour, property::eDouble, 3, false, "0.0f" },
	{ kOfxInteractPropViewportSize, property::eDouble, 2, false, "0.0f" },
	{ kOfxInteractPropPixelScale, property::eDouble, 2, false, "1.0f" },
	{ kOfxInteractPropPenPosition, property::eDouble, 2, false, "0.0" },
	#ifdef kOfxInteractPropPenViewportPosition
	{ kOfxInteractPropPenViewportPosition, property::eInt, 2, false, "0" },
	#endif
	{ kOfxInteractPropPenPressure, property::eDouble, 1, false, "0.0" },
	{ kOfxPropKeyString, property::eString, 1, false, "" },
	{ kOfxPropKeySym, property::eInt, 1, false, "0" },
	{ 0 },
};

// instance

OfxhInteract::OfxhInteract( const OfxhInteractDescriptor& desc, void* effectInstance )
	: _descriptor( desc ),
	_properties( interactInstanceStuffs ),
	_state( desc.getState() ),
	_effectInstance( effectInstance ),
	_argProperties( interactArgsStuffs )
{
	_properties.setPointerProperty( kOfxPropEffectInstance, effectInstance );
	_properties.setChainedSet( &desc.getProperties() ); /// chain it into the descriptor props
	_properties.setGetHook( kOfxInteractPropPixelScale, this );
	_properties.setGetHook( kOfxInteractPropBackgroundColour, this );
	_properties.setGetHook( kOfxInteractPropViewportSize, this );

	_argProperties.setGetHook( kOfxInteractPropPixelScale, this );
	_argProperties.setGetHook( kOfxInteractPropBackgroundColour, this );
	_argProperties.setGetHook( kOfxInteractPropViewportSize, this );
}

OfxhInteract::~OfxhInteract()
{
	/// call it directly incase CI failed and we should always tidy up after create instance
	callEntry( kOfxActionDestroyInstance,  NULL );
}

/// call the entry point in the descriptor with action and the given args
OfxStatus OfxhInteract::callEntry( const char* action, property::OfxhSet* inArgs )
{
	if( _state != eFailed )
	{
		OfxPropertySetHandle inHandle = inArgs ? inArgs->getHandle() : NULL ;
		return _descriptor.callEntry( action, getHandle(), inHandle, NULL );
	}
	return kOfxStatFailed;
}

// do nothing
size_t OfxhInteract::getDimension( const std::string& name ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxInteractPropPixelScale )
	{
		return 2;
	}
	else if( name == kOfxInteractPropBackgroundColour )
	{
		return 3;
	}
	else if( name == kOfxInteractPropViewportSize )
	{
		return 2;
	}
	else
		throw property::OfxhException( kOfxStatErrValue );
}

// do nothing function
void OfxhInteract::reset( const std::string& name ) OFX_EXCEPTION_SPEC
{
	// no-op
}

double OfxhInteract::getDoubleProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxInteractPropPixelScale )
	{
		if( index >= 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double first[2];
		getPixelScale( first[0], first[1] );
		return first[index];
	}
	else if( name == kOfxInteractPropBackgroundColour )
	{
		if( index >= 3 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double first[3];
		getBackgroundColour( first[0], first[1], first[2] );
		return first[index];
	}
	else if( name == kOfxInteractPropViewportSize )
	{
		if( index >= 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		double first[2];
		getViewportSize( first[0], first[1] );
		return first[index];
	}
	else
		throw property::OfxhException( kOfxStatErrUnknown );
}

void OfxhInteract::getDoublePropertyN( const std::string& name, double* first, int n ) const OFX_EXCEPTION_SPEC
{
	if( name == kOfxInteractPropPixelScale )
	{
		if( n > 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getPixelScale( first[0], first[1] );
	}
	else if( name == kOfxInteractPropBackgroundColour )
	{
		if( n > 3 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getBackgroundColour( first[0], first[1], first[2] );
	}
	else if( name == kOfxInteractPropViewportSize )
	{
		if( n > 2 )
			throw property::OfxhException( kOfxStatErrBadIndex );
		getViewportSize( first[0], first[1] );
	}
	else
		throw property::OfxhException( kOfxStatErrUnknown );
}

void OfxhInteract::getSlaveToParam( std::vector<std::string>& params ) const
{
	int nSlaveParams = _properties.getDimension( kOfxInteractPropSlaveToParam );

	for( int i = 0; i < nSlaveParams; i++ )
	{
		std::string param = _properties.getStringProperty( kOfxInteractPropSlaveToParam, i );
		params.push_back( param );
	}
}

/// initialise the argument properties
void OfxhInteract::initArgProp( OfxTime          time,
                            const OfxPointD& renderScale )
{
	double pixelScale[2];

	getPixelScale( pixelScale[0], pixelScale[1] );
	_argProperties.setDoublePropertyN( kOfxPropEffectInstance, pixelScale, 2 );
	_argProperties.setPointerProperty( kOfxPropEffectInstance, _effectInstance );
	_argProperties.setPointerProperty( kOfxPropInstanceData, _properties.getPointerProperty( kOfxPropInstanceData ) );
	_argProperties.setDoubleProperty( kOfxPropTime, time );
	_argProperties.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );
}

void OfxhInteract::setPenArgProps( const OfxPointD& penPos,
                               const OfxPointI& penPosViewport,
                               double           pressure )
{
	_argProperties.setDoublePropertyN( kOfxInteractPropPenPosition, &penPos.x, 2 );
	#ifdef kOfxInteractPropPenViewportPosition
	_argProperties.setIntPropertyN( kOfxInteractPropPenViewportPosition, &penPosViewport.x, 2 );
	#endif
	_argProperties.setDoubleProperty( kOfxInteractPropPenPressure, pressure );
}

void OfxhInteract::setKeyArgProps( int   key,
                               char* keyString )
{
	_argProperties.setIntProperty( kOfxPropKeySym, key );
	_argProperties.setStringProperty( kOfxPropKeyString, keyString );
}

OfxStatus OfxhInteract::createInstanceAction()
{
	OfxStatus stat = callEntry( kOfxActionCreateInstance, NULL );

	if( stat == kOfxStatOK || stat == kOfxStatReplyDefault )
	{
		_state = eCreated;
	}
	else
	{
		_state = eFailed;
	}
	return stat;
}

OfxStatus OfxhInteract::drawAction( OfxTime          time,
                                const OfxPointD& renderScale )
{
	initArgProp( time, renderScale );
	return callEntry( kOfxInteractActionDraw, &_argProperties );
}

OfxStatus OfxhInteract::penMotionAction( OfxTime          time,
                                     const OfxPointD& renderScale,
                                     const OfxPointD& penPos,
                                     const OfxPointI& penPosViewport,
                                     double           pressure )
{
	initArgProp( time, renderScale );
	setPenArgProps( penPos, penPosViewport, pressure );
	return callEntry( kOfxInteractActionPenMotion, &_argProperties );
}

OfxStatus OfxhInteract::penUpAction( OfxTime          time,
                                 const OfxPointD& renderScale,
                                 const OfxPointD& penPos,
                                 const OfxPointI& penPosViewport,
                                 double           pressure )
{
	initArgProp( time, renderScale );
	setPenArgProps( penPos, penPosViewport, pressure );
	return callEntry( kOfxInteractActionPenUp, &_argProperties );
}

OfxStatus OfxhInteract::penDownAction( OfxTime          time,
                                   const OfxPointD& renderScale,
                                   const OfxPointD& penPos,
                                   const OfxPointI& penPosViewport,
                                   double           pressure )
{
	initArgProp( time, renderScale );
	setPenArgProps( penPos, penPosViewport, pressure );
	return callEntry( kOfxInteractActionPenDown, &_argProperties );
}

OfxStatus OfxhInteract::keyDownAction( OfxTime          time,
                                   const OfxPointD& renderScale,
                                   int              key,
                                   char*            keyString )
{
	initArgProp( time, renderScale );
	setKeyArgProps( key, keyString );
	return callEntry( kOfxInteractActionKeyDown, &_argProperties );
}

OfxStatus OfxhInteract::keyUpAction( OfxTime          time,
                                 const OfxPointD& renderScale,
                                 int              key,
                                 char*            keyString )
{
	initArgProp( time, renderScale );
	setKeyArgProps( key, keyString );
	return callEntry( kOfxInteractActionKeyUp, &_argProperties );
}

OfxStatus OfxhInteract::keyRepeatAction( OfxTime          time,
                                     const OfxPointD& renderScale,
                                     int              key,
                                     char*            keyString )
{
	initArgProp( time, renderScale );
	setKeyArgProps( key, keyString );
	return callEntry( kOfxInteractActionKeyRepeat, &_argProperties );
}

OfxStatus OfxhInteract::gainFocusAction( OfxTime          time,
                                     const OfxPointD& renderScale )
{
	initArgProp( time, renderScale );
	return callEntry( kOfxInteractActionGainFocus, &_argProperties );
}

OfxStatus OfxhInteract::loseFocusAction( OfxTime          time,
                                     const OfxPointD& renderScale )
{
	initArgProp( time, renderScale );
	return callEntry( kOfxInteractActionLoseFocus, &_argProperties );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Interact suite functions

static OfxStatus interactSwapBuffers( OfxInteractHandle handle )
{
	interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>( handle );

	if( interactInstance )
		return interactInstance->swapBuffers();
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus interactRedraw( OfxInteractHandle handle )
{
	interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>( handle );

	if( interactInstance )
		return interactInstance->redraw();
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus interactGetPropertySet( OfxInteractHandle handle, OfxPropertySetHandle* property )
{
	interact::OfxhBase* interact = reinterpret_cast<interact::OfxhBase*>( handle );

	if( interact )
	{
		*property = interact->getPropHandle();
		return kOfxStatOK;
	}
	return kOfxStatErrBadHandle;
}

/// the interact suite
static OfxInteractSuiteV1 gSuite = {
	interactSwapBuffers,
	interactRedraw,
	interactGetPropertySet
};

/// function to get the sutie
void* GetSuite( int version )
{
	if( version == 1 )
		return (void*) &gSuite;
	return NULL;
}

}
}
}
}
