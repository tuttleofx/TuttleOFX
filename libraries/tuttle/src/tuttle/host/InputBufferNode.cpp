#include "InputBufferNode.hpp"
#include "Core.hpp"
#include "exceptions.hpp"

#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>

namespace tuttle {
namespace host {

const std::string InputBufferNode::_label( "InputBufferNode" );

const ofx::attribute::OfxhClipImageDescriptor _clipDesc( kOfxOutputAttributeName );

InputBufferNode::InputBufferNode( )
: _name("InputBufferNodeWithoutName")
, _outputClip( *this, _clipDesc )
{
}

InputBufferNode::InputBufferNode( const InputBufferNode& other )
: _name(other._name)
, _outputClip( other._outputClip )
{ }

InputBufferNode::~InputBufferNode( ) { }

attribute::Attribute& InputBufferNode::getAttribute( const std::string& name )
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No attribute on InputBufferNode." ) );
}

attribute::Attribute&       InputBufferNode::getSingleInputAttribute()
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No attribute on InputBufferNode." ) );
}

const attribute::Attribute& InputBufferNode::getSingleInputAttribute() const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No attribute on InputBufferNode." ) );
}

const ofx::attribute::OfxhParam& InputBufferNode::getParam( const std::string& name ) const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

ofx::attribute::OfxhParam&       InputBufferNode::getParam( const std::string& name )
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}


attribute::ClipImage&       InputBufferNode::getClip( const std::string& name )
{
	if( name != kOfxImageEffectOutputClipName )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "No input clip on InputBufferNode (\"" + name + "\")." );
	}
	return _outputClip;
}

const attribute::ClipImage&       InputBufferNode::getClip( const std::string& name ) const
{
	if( name != kOfxImageEffectOutputClipName )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "No input clip on InputBufferNode (\"" + name + "\")." );
	}
	return _outputClip;
}

ofx::attribute::OfxhParamSet& InputBufferNode::getParamSet()
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No ParamSet on InputBufferNode." ) );
}

const ofx::attribute::OfxhParamSet& InputBufferNode::getParamSet() const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No ParamSet on InputBufferNode." ) );
}


std::ostream& InputBufferNode::print( std::ostream& os ) const
{
	const InputBufferNode& v = *this;
	os << "________________________________________________________________________________" << std::endl;
	os << "Plug-in:" << v.getLabel() << std::endl;
	os << "________________________________________________________________________________" << std::endl;
	return os;
}


void InputBufferNode::process( graph::ProcessVertexAtTimeData& vData )
{
	memory::IMemoryCache& memoryCache( Core::instance().getMemoryCache() );
	memory::CACHE_ELEMENT image;
	image.reset( new attribute::Image( _outputClip, vData._apiImageEffect._renderRoI, vData._time ) );
	memoryCache.put( _outputClip.getIdentifier(), vData._time, image );
}


std::ostream& operator<<( std::ostream& os, const InputBufferNode& v )
{
	return v.print(os);
}

}
}

