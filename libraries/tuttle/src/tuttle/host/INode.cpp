#include "INode.hpp"

#include "ImageEffectNode.hpp"

#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

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

std::string INode::getVersionStr() const
{
	std::vector<int> vInt = getVersion();
	std::vector<std::string> vStr;
	BOOST_FOREACH( const int v, vInt )
	{
		vStr.push_back( boost::lexical_cast<std::string>(v) );
	}
	return boost::algorithm::join( vStr, "." );
}

void INode::setProcessData( Data* data )
{
	_data = data;
}

void INode::setProcessDataAtTime( DataAtTime* dataAtTime )
{
	TUTTLE_LOG_TRACE( "setProcessDataAtTime \"" << getName() << "\" at " << dataAtTime->_time );
	_dataAtTime[dataAtTime->_time] = dataAtTime;
}

void INode::clearProcessDataAtTime()
{
	_dataAtTime.clear();
}

void INode::setBeforeRenderCallback( Callback *cb )
{
    _beforeRenderCallback = cb;
}

void INode::beforeRenderCallback( INode &node, DataAtTime &data )
{
    if( _beforeRenderCallback )
    {
        _beforeRenderCallback->call(node, data._time);
    }
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

bool INode::hasData( const OfxTime time ) const
{
	DataAtTimeMap::const_iterator it = _dataAtTime.find( time );
	return it != _dataAtTime.end();
}

const INode::DataAtTime& INode::getData( const OfxTime time ) const
{
	//TUTTLE_LOG_TRACE( "- INode::getData(" << time << ") of " << getName() );
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
			//<< exception::pluginIdentifier( getPlugin().getIdentifier() )
			<< exception::time( time ) );
	}
	return *it->second;
}

INode::DataAtTime& INode::getData( const OfxTime time )
{
	return const_cast<DataAtTime&>( const_cast<const This*>(this)->getData(time) );
}

const INode::DataAtTime& INode::getFirstData() const
{
	DataAtTimeMap::const_iterator it = _dataAtTime.begin();
	if( it == _dataAtTime.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "Process data empty."
			<< exception::nodeName( getName() ) );
	}
	return *it->second;
}

INode::DataAtTime& INode::getFirstData()
{
	return const_cast<DataAtTime&>( const_cast<const This*>(this)->getFirstData() );
}

const INode::DataAtTime& INode::getLastData() const
{
	DataAtTimeMap::const_reverse_iterator it = _dataAtTime.rbegin();
	if( it == _dataAtTime.rend() )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev() + "Process data empty."
			<< exception::nodeName( getName() ) );
	}
	return *it->second;
}

INode::DataAtTime& INode::getLastData()
{
	return const_cast<DataAtTime&>( const_cast<const This*>(this)->getLastData() );
}

std::ostream& operator<<( std::ostream& os, const INode& v )
{
	return v.print(os);
}

}
}
