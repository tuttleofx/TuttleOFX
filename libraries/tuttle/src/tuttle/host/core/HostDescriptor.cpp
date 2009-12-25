/*
 * Software License :
 *
 * Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.
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

#include <iostream>
#include <fstream>
#include <cstring>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhProperty.hpp>
#include <tuttle/host/ofx/OfxhClip.hpp>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

// my host
#include "HostDescriptor.hpp"
#include "ImageEffectNode.hpp"
#include "ClipImage.hpp"

namespace tuttle {
namespace host {
namespace core {

Host::Host()
{
	/// @todo tuttle set host properties correctly...
	_properties.setStringProperty( kOfxPropName, "TuttleOfx" );
	_properties.setStringProperty( kOfxPropLabel, "TuttleOfx alpha" );
	_properties.setIntProperty( kOfxImageEffectHostPropIsBackground, 1 );
	_properties.setIntProperty( kOfxImageEffectPropSupportsOverlays, 0 );
	_properties.setIntProperty( kOfxImageEffectPropSupportsMultiResolution, 0 );
	_properties.setIntProperty( kOfxImageEffectPropSupportsTiles, false /*true*/ ); ///< we hope to do this !
	_properties.setIntProperty( kOfxImageEffectPropTemporalClipAccess, false /*true*/ ); ///< we hope to do this !
	_properties.setStringProperty( kOfxImageEffectPropSupportedComponents,  kOfxImageComponentRGBA, 0 );
	_properties.setStringProperty( kOfxImageEffectPropSupportedComponents,  kOfxImageComponentAlpha, 1 );
	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextFilter, 0 );
	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGenerator, 1 );
	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGeneral, 2 );
//	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextTransition, 3 ); ///< we hope to do this !
	_properties.setIntProperty( kOfxImageEffectPropSupportsMultipleClipDepths, 1 );
	_properties.setIntProperty( kOfxImageEffectPropSupportsMultipleClipPARs, 1 );
	_properties.setIntProperty( kOfxImageEffectPropSetableFrameRate, 0 );
	_properties.setIntProperty( kOfxImageEffectPropSetableFielding, 0 );
	_properties.setIntProperty( kOfxParamHostPropSupportsCustomInteract, 0 );
	_properties.setIntProperty( kOfxParamHostPropSupportsStringAnimation, 0 );
	_properties.setIntProperty( kOfxParamHostPropSupportsChoiceAnimation, 0 );
	_properties.setIntProperty( kOfxParamHostPropSupportsBooleanAnimation, 0 );
	_properties.setIntProperty( kOfxParamHostPropSupportsCustomAnimation, 0 );
	_properties.setIntProperty( kOfxParamHostPropMaxParameters, -1 );
	_properties.setIntProperty( kOfxParamHostPropMaxPages, 0 );
	_properties.setIntProperty( kOfxParamHostPropPageRowColumnCount, 0, 0 );
	_properties.setIntProperty( kOfxParamHostPropPageRowColumnCount, 0, 1 );
}

tuttle::host::core::ImageEffectNode* Host::newInstance( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin,
                                                        tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor&        desc,
                                                        const std::string&                                     context ) const
{
	return new tuttle::host::core::ImageEffectNode( plugin, desc, context );
}

/// Override this to create a descriptor, this makes the 'root' descriptor
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* Host::makeDescriptor( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin ) const
{
	tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* desc = new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor( plugin );

	return desc;
}

/// used to construct a context description, rootContext is the main context
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* Host::makeDescriptor( const tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor&      rootContext,
                                                                  tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin ) const
{
	return new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor( rootContext, plugin );
}

/// used to construct populate the cache
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* Host::makeDescriptor( const std::string&                                     bundlePath,
                                                                  tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin ) const
{
	return new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor( bundlePath, plugin );
}

/// message
OfxStatus Host::vmessage( const char* type,
                          const char* id,
                          const char* format,
                          va_list     args ) const
{
	bool isQuestion    = false;
	const char* prefix = "Message : ";

	if( strcmp( type, kOfxMessageLog ) == 0 )
	{
		prefix = "Log : ";
	}
	else if( strcmp( type, kOfxMessageFatal ) == 0 ||
	         strcmp( type, kOfxMessageError ) == 0 )
	{
		prefix = "Error : ";
	}
	else if( strcmp( type, kOfxMessageQuestion ) == 0 )
	{
		prefix     = "Question : ";
		isQuestion = true;
	}

	// Just dump our message to stdout, should be done with a proper
	// UI in a full ap, and post a dialogue for yes/no questions.
	fputs( prefix, stdout );
	vprintf( format, args );
	printf( "\n" );

	if( isQuestion )
	{
		/// cant do this properly inour example, as we need to raise a dialogue to ask a question, so just return yes
		return kOfxStatReplyYes;
	}
	else
	{
		return kOfxStatOK;
	}
}

}
}
}
