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
#ifndef _TUTTLE_HOST_DESCRIPTOR_H
#define _TUTTLE_HOST_DESCRIPTOR_H

#include <tuttle/host/ofx/OfxhImageEffect.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

/** a host combines several things...
 *    - a factory to create a new instance of your plugin
 *      - it also gets to filter some calls during in the
 *         API to check for validity and perform custom
 *         operations (eg: add extra properties).
 *     - it provides a description of the host application
 *       which is passed back to the plugin.
 *
 * @todo how to support multiple APIs...
 */
class Host : public tuttle::host::ofx::imageEffect::OfxhImageEffectHost
{
public:
	Host();

	/** Create a new instance of an image effect plug-in.
	 *
	 *  It is called by ImageEffectPlugin::createInstance which the
	 *  client code calls when it wants to make a new instance.
	 *
	 *  @arg clientData - the clientData passed into the ImageEffectPlugin::createInstance
	 *  @arg plugin - the plugin being created
	 *  @arg desc - the descriptor for that plugin
	 *  @arg context - the context to be created in
	 */
	tuttle::host::core::ImageEffectNode* newInstance( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin,
	                                                  tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor&        desc,
	                                                  const std::string&                                     context ) const;
	/// Override this to create a descriptor, this makes the 'root' descriptor
	tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* makeDescriptor( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin ) const;

	/// used to construct a context description, rootContext is the main context
	tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* makeDescriptor( const tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor&      rootContext,
	                                                            tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plug ) const;

	/// used to construct populate the cache
	tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* makeDescriptor( const std::string&                                     bundlePath,
	                                                            tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plug ) const;

	/// vmessage
	OfxStatus vmessage( const char* type,
	                    const char* id,
	                    const char* format,
	                    va_list     args ) const;

};

}
}
}

#endif
