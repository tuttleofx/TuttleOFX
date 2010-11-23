#include "INode.hpp"

#include "ImageEffectNode.hpp"
#include "InputBufferNode.hpp"

#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {

INode::~INode() {}

ImageEffectNode& INode::asImageEffectNode( )
{
	return dynamic_cast<ImageEffectNode&> ( *this );
}

const ImageEffectNode& INode::asImageEffectNode( ) const
{
	return dynamic_cast<const ImageEffectNode&> ( *this );
}

InputBufferNode& INode::asInputBufferNode( )
{
	return dynamic_cast<InputBufferNode&> ( *this );
}

const InputBufferNode& INode::asInputBufferNode( ) const
{
	return dynamic_cast<const InputBufferNode&> ( *this );
}


void INode::setProcessData( Data* data )
{
	_data = data;
}

void INode::setProcessData( DataAtTime* dataAtTime )
{
	_dataAtTime[dataAtTime->_time] = dataAtTime;
}

INode::Data& INode::getData()
{
	if( !_data )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev("Process data not set.")
			<< exception::nodeName( getName() ) );
	}
	return *_data;
}

const INode::Data& INode::getData() const
{
	if( !_data )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev("Process data not set.")
			<< exception::nodeName( getName() ) );
	}
	return *_data;
}

const INode::DataAtTime& INode::getData( const OfxTime time ) const
{
	DataAtTimeMap::const_iterator it = _dataAtTime.find( time );
	if( it == _dataAtTime.end() )
	{
		std::ostringstream ss;
		ss << "Defined times : ";
		BOOST_FOREACH( const DataAtTimeMap::value_type& v, _dataAtTime )
		{
			ss << v.first << ", ";
		}
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "Process data at time not set.\n"
								+ ss.str()
			<< exception::nodeName( getName() )
//			<< exception::pluginIdentifier( getPlugin().getIdentifier() )
			<< exception::time( time ) );
	}
	return *it->second;
}

INode::DataAtTime& INode::getData( const OfxTime time )
{
	return const_cast<DataAtTime&>( const_cast<const This*>(this)->getData(time) );
}

std::ostream& operator<<( std::ostream& os, const INode& v )
{
	return v.print(os);
}

}
}
