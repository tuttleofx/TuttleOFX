/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
	  contributors may be used to endorse or promote products derived from this
	  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <cstdio>

// ofx
#include "ofxCore.h"
#include "ofxClip.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhImageEffect.h"

namespace OFX {

	namespace Host {

		namespace Attribute {

				////////////////////////////////////////////////////////////////////////////////
				// props to clips descriptors and instances

				/**
				 * base ctor, for a descriptor
				 */
				ClipImageAccessor::ClipImageAccessor()
				{
				}

				ClipImageAccessor::~ClipImageAccessor(){}

				/** return a std::vector of supported comp
				 */
				const std::vector<std::string> &ClipImageAccessor::getSupportedComponents( ) const
				{
					Property::String *p = getProperties().fetchStringProperty( kOfxImageEffectPropSupportedComponents );
					assert( p != NULL );
					return p->getValues( );
				}

				/** is the given component supported
				 */
				bool ClipImageAccessor::isSupportedComponent( const std::string &comp ) const
				{
					return getProperties().findStringPropValueIndex( kOfxImageEffectPropSupportedComponents, comp ) != -1;
				}

				/** does the clip do random temporal access
				 */
				bool ClipImageAccessor::temporalAccess( ) const
				{
					return getProperties().getIntProperty( kOfxImageEffectPropTemporalClipAccess ) != 0;
				}

				/** is the clip a nominal 'mask' clip
				 */
				bool ClipImageAccessor::isMask( ) const
				{
					return getProperties().getIntProperty( kOfxImageClipPropIsMask ) != 0;
				}

				/** how does this clip like fielded images to be presented to it
				 */
				const std::string &ClipImageAccessor::getFieldExtraction( ) const
				{
					return getProperties().getStringProperty( kOfxImageClipPropFieldExtraction );
				}

				/** is the clip a nominal 'mask' clip
				 */
				bool ClipImageAccessor::supportsTiles( ) const
				{
					return getProperties().getIntProperty( kOfxImageEffectPropSupportsTiles ) != 0;
				}

				/**
				 * descriptor
				 */
				ClipImageDescriptor::ClipImageDescriptor( const std::string& name )
				: Attribute::ClipDescriptor( Property::Set() )
				{
					/// properties common to the desciptor and instance
					/// the desc and set them, the instance cannot
					static const Property::PropSpec clipImageDescriptorStuffs[] = {
						{ kOfxClipPropType, Property::eString, 1, true, kOfxClipTypeImage },
						{ kOfxImageEffectPropSupportedComponents, Property::eString, 0, false, "" },
						{ kOfxImageEffectPropTemporalClipAccess, Property::eInt, 1, false, "0" },
						{ kOfxImageClipPropIsMask, Property::eInt, 1, false, "0" },
						{ kOfxImageClipPropFieldExtraction, Property::eString, 1, false, kOfxImageFieldDoubled },
						{ kOfxImageEffectPropSupportsTiles, Property::eInt, 1, false, "1" },
						{ 0 },
					};
					_properties.addProperties( clipImageDescriptorStuffs );
					_properties.setStringProperty( kOfxPropName, name );
				}

				/**
				 * clip clipimage instance
				 */
				ClipImageInstance::ClipImageInstance( ImageEffect::Instance* effectInstance, const Attribute::ClipImageDescriptor& desc )
				: Attribute::ClipInstance( desc )
				, _effectInstance( effectInstance )
//				, _pixelDepth( kOfxBitDepthNone )
//				, _components( kOfxImageComponentNone )
				{
//					_par = 1.0;
					/**
					 * extra properties for the instance, these are fetched from the host
					 * via a get hook and some virtuals
					 */
					static Property::PropSpec clipImageInstanceStuffs[] = {
						{ kOfxImageEffectPropPixelDepth, Property::eString, 1, true, kOfxBitDepthNone },
						{ kOfxImageEffectPropComponents, Property::eString, 1, true, kOfxImageComponentNone },
						{ kOfxImageClipPropUnmappedPixelDepth, Property::eString, 1, true, kOfxBitDepthNone },
						{ kOfxImageClipPropUnmappedComponents, Property::eString, 1, true, kOfxImageComponentNone },
						{ kOfxImageEffectPropPreMultiplication, Property::eString, 1, true, kOfxImageOpaque },
						{ kOfxImagePropPixelAspectRatio, Property::eDouble, 1, true, "1.0" },
						{ kOfxImageEffectPropFrameRate, Property::eDouble, 1, true, "25.0" },
						{ kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
						{ kOfxImageClipPropFieldOrder, Property::eString, 1, true, kOfxImageFieldNone },
						{ kOfxImageEffectPropUnmappedFrameRange, Property::eDouble, 2, true, "0" },
						{ kOfxImageEffectPropUnmappedFrameRate, Property::eDouble, 1, true, "25.0" },
						{ kOfxImageClipPropContinuousSamples, Property::eInt, 1, true, "0" },
						{ 0 },
					};

					_properties.addProperties( clipImageInstanceStuffs );
					initHook( clipImageInstanceStuffs );
				}

				OfxStatus ClipImageInstance::instanceChangedAction( std::string why,
															   OfxTime time,
															   OfxPointD renderScale )
				{
					Property::PropSpec stuff[] = {
						{ kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
						{ kOfxPropName, Property::eString, 1, true, getName( ).c_str( ) },
						{ kOfxPropChangeReason, Property::eString, 1, true, why.c_str( ) },
						{ kOfxPropTime, Property::eDouble, 1, true, "0" },
						{ kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
						{ 0 }
					};

					Property::Set inArgs( stuff );

					// add the second dimension of the render scale
					inArgs.setDoubleProperty( kOfxPropTime, time );
					inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

					if( _effectInstance )
					{
						return _effectInstance->mainEntry( kOfxActionInstanceChanged, _effectInstance->getHandle( ), &inArgs, 0 );
					}

					return kOfxStatFailed;
				}

				/// given the colour component, find the nearest set of supported colour components

				const std::string &ClipImageInstance::findSupportedComp( const std::string &s ) const
				{
					static const std::string none( kOfxImageComponentNone );
					static const std::string rgba( kOfxImageComponentRGBA );
					static const std::string alpha( kOfxImageComponentAlpha );

					/// is it there
					if( isSupportedComponent( s ) )
						return s;

					/// were we fed some custom non chromatic component by getUnmappedComponents? Return it.
					/// we should never be here mind, so a bit weird
					if( !_effectInstance->isChromaticComponent( s ) )
						return s;

					/// Means we have RGBA or Alpha being passed in and the clip
					/// only supports the other one, so return that
					if( s == kOfxImageComponentRGBA && isSupportedComponent( kOfxImageComponentAlpha ) )
						return alpha;

					if( s == kOfxImageComponentAlpha && isSupportedComponent( kOfxImageComponentRGBA ) )
						return rgba;

					/// wierd, must be some custom bit , if only one, choose that, otherwise no idea
					/// how to map, you need to derive to do so.
					const std::vector<std::string> &supportedComps = getSupportedComponents( );
					if( supportedComps.size( ) == 1 )
						return supportedComps[0];

					return none;
				}

		} // Attribute

	} // Host

} // OFX
