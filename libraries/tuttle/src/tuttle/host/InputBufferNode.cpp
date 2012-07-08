#include "InputBufferNode.hpp"
#include "Core.hpp"
#include "exceptions.hpp"
#include "memory/LinkData.hpp"

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


InputBufferNode::~InputBufferNode( ) { }

InputBufferNode::InputBufferNode( const InputBufferNode& other )
: _name(other._name)
, _outputClip( other._outputClip )
{ }

bool InputBufferNode::operator==( const INode& other ) const
{
	const InputBufferNode* other_ptr = dynamic_cast<const InputBufferNode*>( &other );
	if( other_ptr == NULL )
		return false;
	return operator==( *other_ptr );
}

bool InputBufferNode::operator==( const InputBufferNode& other ) const
{
	return ( _name == other._name );
}

std::vector<int> InputBufferNode::getVersion() const
{
	std::vector<int> version;
	version.push_back( TUTTLE_HOST_VERSION_MAJOR );
	version.push_back( TUTTLE_HOST_VERSION_MINOR );
	version.push_back( TUTTLE_HOST_VERSION_MICRO );
	return version;
}

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

ofx::attribute::OfxhParam&       InputBufferNode::getParam( const std::string& name )
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

const ofx::attribute::OfxhParam& InputBufferNode::getParam( const std::string& name ) const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

ofx::attribute::OfxhParam&       InputBufferNode::getParamByScriptName( const std::string& name, const bool acceptPartialName )
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

const ofx::attribute::OfxhParam& InputBufferNode::getParamByScriptName( const std::string& name, const bool acceptPartialName ) const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

ofx::attribute::OfxhParam&       InputBufferNode::getParam( const std::size_t index )
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

const ofx::attribute::OfxhParam& InputBufferNode::getParam( const std::size_t index ) const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No param on InputBufferNode." ) );
}

attribute::ClipImage&       InputBufferNode::getClip( const std::string& name, const bool acceptPartialName )
{
	if( name != kOfxImageEffectOutputClipName )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "No input clip on InputBufferNode (\"" + name + "\")." );
	}
	return _outputClip;
}

const attribute::ClipImage&       InputBufferNode::getClip( const std::string& name, const bool acceptPartialName ) const
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

ofx::attribute::OfxhClipImageSet& InputBufferNode::getClipImageSet()
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No ClipImageSet on InputBufferNode." ) );
}

const ofx::attribute::OfxhClipImageSet& InputBufferNode::getClipImageSet() const
{
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev( "No ClipImageSet on getClipImageSet." ) );
}

void InputBufferNode::setRawImageBuffer(
		char* rawBuffer,
		const OfxRectD& rod,
		const ofx::imageEffect::EPixelComponent components,
		const ofx::imageEffect::EBitDepth bitDepth,
		const int rowDistanceBytes,
		const attribute::Image::EImageOrientation orientation )
{
	_outputClip.setComponents( components );
	_outputClip.setBitDepth( bitDepth );
	_imageCache.reset( new attribute::Image(
			_outputClip,
			0, // no time information
			rod,
			orientation,
			rowDistanceBytes
		) );
	_imageCache->setPoolData( new memory::LinkData( rawBuffer ) );
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
	Core::instance().getMemoryCache().put( _outputClip.getClipIdentifier(), vData._time, _imageCache );
	if( vData._outDegree > 0 )
	{
		_imageCache->addReference( vData._outDegree ); // add a reference on this node for each future usages
	}
}


std::ostream& operator<<( std::ostream& os, const InputBufferNode& v )
{
	return v.print(os);
}

}
}

