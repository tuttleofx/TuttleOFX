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


// ofx host
#include "OfxhBinary.hpp"
#include "OfxhMemory.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"
#include "OfxhImageEffectPlugin.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhImageEffectPluginCache.hpp"
#include "property/OfxhSet.hpp"
#include "attribute/OfxhClip.hpp"
#include "attribute/OfxhParam.hpp"

///@todo tuttle: remove this !
#include <tuttle/host/Core.hpp>
#include <tuttle/host/serialization.hpp>

// ofx
#include <ofxImageEffect.h>

#include <string>
#include <map>

// Disable the "this pointer used in base member initialiser list" warning in Windows
namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( disable : 4355 )
#endif

OfxhImageEffectPlugin::OfxhImageEffectPlugin()
	: OfxhPlugin()
	, _pc( NULL )
	, _pluginHandle( NULL )
	, _baseDescriptor( NULL )
{
	/// @todo tuttle
}

OfxhImageEffectPlugin::OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc, OfxhPluginBinary& pb, int pi, OfxPlugin& pl )
	: OfxhPlugin( pb, pi, pl )
	, _pc( &pc )
	, _pluginHandle( 0 )
	, _baseDescriptor( Core::instance().getHost().makeDescriptor( *this ) )
{
	//	loadAndDescribeActions();
}

OfxhImageEffectPlugin::OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc,
                                              OfxhPluginBinary&           pb,
                                              int                         pi,
                                              const std::string&          api,
                                              int                         apiVersion,
                                              const std::string&          pluginId,
                                              const std::string&          rawId,
                                              int                         pluginMajorVersion,
                                              int                         pluginMinorVersion )
	: OfxhPlugin( pb, pi, api, apiVersion, pluginId, rawId, pluginMajorVersion, pluginMinorVersion )
	, _pc( &pc )
	, _pluginHandle( NULL )
	, _baseDescriptor( Core::instance().getHost().makeDescriptor( *this ) )
{
	//	loadAndDescribeActions();
}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( default : 4355 )
#endif

OfxhImageEffectPlugin::~OfxhImageEffectPlugin()
{
	if( getPluginHandle() )
	{
		getPluginHandle()->getOfxPlugin()->mainEntry( kOfxActionUnload, 0, 0, 0 );
	}
}

bool OfxhImageEffectPlugin::operator==( const OfxhImageEffectPlugin& other ) const
{
	if( OfxhPlugin::operator!=( other ) ||
	    *_baseDescriptor != *( other._baseDescriptor ) )
		return false;
	return true;
}

void OfxhImageEffectPlugin::setApiHandler( APICache::OfxhPluginAPICacheI& api )
{
	_pc = &dynamic_cast<OfxhImageEffectPluginCache&>( api );
}

APICache::OfxhPluginAPICacheI& OfxhImageEffectPlugin::getApiHandler()
{
	return *_pc;
}

const APICache::OfxhPluginAPICacheI& OfxhImageEffectPlugin::getApiHandler() const
{
	return *_pc;
}

/// get the image effect descriptor
OfxhImageEffectNodeDescriptor& OfxhImageEffectPlugin::getDescriptor()
{
	return *_baseDescriptor;
}

/// get the image effect descriptor const version
const OfxhImageEffectNodeDescriptor& OfxhImageEffectPlugin::getDescriptor() const
{
	return *_baseDescriptor;
}

void OfxhImageEffectPlugin::addContext( const std::string& context, OfxhImageEffectNodeDescriptor* ied )
{
	std::string key( context ); // for constness

	_contexts.insert( key, ied );
	_knownContexts.insert( context );
}

void OfxhImageEffectPlugin::addContext( const std::string& context )
{
	_knownContexts.insert( context );
	//TCOUT( "OfxhImageEffectPlugin::addContext " << context << " on plugin " << this->getRawIdentifier() );
}

const std::set<std::string>& OfxhImageEffectPlugin::getContexts() const
{
	return _knownContexts;
}

bool OfxhImageEffectPlugin::supportsContext( const std::string& context ) const
{
	/*
	   std::cout << context << " supportsContext? " << _knownContexts.size() << std::endl;

	   for( ContextSet::iterator it = _knownContexts.begin(),
	     itEnd = _knownContexts.end();
	     it != itEnd;
	 ++it )
	   {
	    TCOUT( "context " << *it );
	   }
	 */
	return _knownContexts.find( context ) != _knownContexts.end();
}

void OfxhImageEffectPlugin::initContexts()
{
	const tuttle::host::ofx::property::OfxhSet& eProps = getDescriptor().getProperties();
	int size                                           = eProps.getDimension( kOfxImageEffectPropSupportedContexts );

	for( int j = 0; j < size; ++j )
	{
		std::string context = eProps.getStringProperty( kOfxImageEffectPropSupportedContexts, j );
		addContext( context );
	}
}

void OfxhImageEffectPlugin::loadAndDescribeActions()
{
	if( getPluginHandle() )
	{
		//COUT( "loadAndDescribeAction already called on plugin " + getApiHandler()._infos._apiName );
		return;
	}
	_pluginHandle.reset( new tuttle::host::ofx::OfxhPluginHandle( *this, getApiHandler().getHost() ) );

	OfxPlugin* op = _pluginHandle->getOfxPlugin();

	if( op == NULL )
	{
		_pluginHandle.reset( NULL );
		BOOST_THROW_EXCEPTION( exception::Data()
		    << exception::dev( "loadAndDescribeAction: OfxPlugin is NULL." )
		    << exception::ofxApi( getApiHandler()._apiName ) );
	}

	int rval = op->mainEntry( kOfxActionLoad, 0, 0, 0 );

	if( rval != kOfxStatOK && rval != kOfxStatReplyDefault )
	{
		_pluginHandle.reset( NULL );
		BOOST_THROW_EXCEPTION( exception::Data()
		    << exception::dev( "Load Action failed." )
		    << exception::ofxApi( getApiHandler()._apiName ) );
	}

	rval = op->mainEntry( kOfxActionDescribe, getDescriptor().getHandle(), 0, 0 );

	if( rval != kOfxStatOK && rval != kOfxStatReplyDefault )
	{
		_pluginHandle.reset( NULL );
		BOOST_THROW_EXCEPTION( exception::Data()
		    << exception::dev( "Describe Action failed." )
		    << exception::ofxApi( getApiHandler()._apiName ) );
	}
	initContexts();
}

OfxhImageEffectNodeDescriptor& OfxhImageEffectPlugin::getDescriptorInContext( const std::string& context )
{
	ContextMap::iterator it = _contexts.find( context );

	//COUT( "context : " << context );
	if( it != _contexts.end() )
	{
		//COUT( "found context description : " << it->second->getLabel() );
		return *( it->second );
	}

	if( _knownContexts.find( context ) == _knownContexts.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
		    << exception::dev( "Context not found." )
		    << exception::ofxContext( context ) );
	}
	return describeInContextAction( context );
}

OfxhImageEffectNodeDescriptor& OfxhImageEffectPlugin::describeInContextAction( const std::string& context )
{
	tuttle::host::ofx::property::OfxhPropSpec inargspec[] = {
		{ kOfxImageEffectPropContext, tuttle::host::ofx::property::eString, 1, true, context.c_str() },
		{ 0 }
	};

	tuttle::host::ofx::property::OfxhSet inarg( inargspec );

	OfxhPluginHandle* ph = getPluginHandle();

	std::auto_ptr<tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor> newContext( Core::instance().getHost().makeDescriptor( getDescriptor(), *this ) );
	int rval = kOfxStatFailed;
	if( ph->getOfxPlugin() )
		rval = ph->getOfxPlugin()->mainEntry( kOfxImageEffectActionDescribeInContext, newContext->getHandle(), inarg.getHandle(), 0 );

	if( rval != kOfxStatOK && rval != kOfxStatReplyDefault )
	{
		BOOST_THROW_EXCEPTION( OfxhException( rval, "kOfxImageEffectActionDescribeInContext failed." ) );
	}
	std::string key( context ); // for constness
	_contexts.insert( key, newContext.release() );
	return _contexts.at( context );
}

imageEffect::OfxhImageEffectNode* OfxhImageEffectPlugin::createInstance( const std::string& context )
{
	/**
	 * @todo - we need to make sure action:load is called, then action:describe again
	 * (not because we are expecting the results to change, but because plugin
	 * might get confused otherwise), then a describe_in_context
	 */
	loadAndDescribeActions();
	if( getPluginHandle() == NULL )
	{
		BOOST_THROW_EXCEPTION( exception::BadHandle() );
	}
	OfxhImageEffectNodeDescriptor& desc        = getDescriptorInContext( context );
	imageEffect::OfxhImageEffectNode* instance = Core::instance().getHost().newInstance( *this, desc, context ); /// @todo tuttle: don't use singleton here.
	instance->createInstanceAction(); // Is it not possible to move this in a constructor ? In some cases it's interesting to initialize host side values before creation of plugin side objets (eg. node duplication or creation from file).
	return instance;
}

void OfxhImageEffectPlugin::unloadAction()
{
	if( getPluginHandle() )
	{
		( *_pluginHandle )->mainEntry( kOfxActionUnload, 0, 0, 0 );
	}
}

}
}
}
}

BOOST_CLASS_EXPORT_IMPLEMENT( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin )

