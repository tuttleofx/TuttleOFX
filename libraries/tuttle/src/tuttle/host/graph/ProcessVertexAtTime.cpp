#include "ProcessVertexAtTime.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

#include <boost/functional/hash.hpp>


namespace tuttle {
namespace host {
namespace graph {


bool ProcessVertexAtTime::Key::operator==( const Key& v ) const
{
	return
		( v._name == _name ) &&
		( v._time == _time );
}

std::size_t ProcessVertexAtTime::Key::getHash() const
{
	std::size_t seed = 0;
	boost::hash_combine( seed, _name );
	boost::hash_combine( seed, _time );
	return seed;
}



ProcessVertexAtTime::ProcessVertexAtTime( )
{ }

ProcessVertexAtTime::ProcessVertexAtTime( const ProcessVertex& v, const OfxTime t )
: IVertex( v )
, _clipName( v._name )
, _data( v._data, t )
{
	this->_name += "_at_" + boost::lexical_cast<std::string>(t);
}

ProcessVertexAtTime::ProcessVertexAtTime( const ProcessVertexAtTime& other )
: IVertex( other )
, _clipName( other._clipName )
, _data( other._data )
{
}

ProcessVertexAtTime::~ProcessVertexAtTime( )
{
}

std::ostream& ProcessVertexAtTime::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	if( ! isFake() )
	{
		/// @todo remove this. Temporary solution
		const ImageEffectNode& ieNode = dynamic_cast<const ImageEffectNode&>( getProcessNode() );
		s << subDotEntry( "bitdepth",  ieNode.getOutputClip().getBitDepthString()   );
		s << subDotEntry( "component", ieNode.getOutputClip().getComponentsString() );
		{
			double startFrame, endFrame;
			ieNode.getOutputClip().getFrameRange( startFrame, endFrame );
			s << subDotEntry( "startFrame", startFrame );
			s << subDotEntry( "endFrame", endFrame );
		}
		s << subDotEntry( "output RoD", ieNode.getOutputClip().fetchRegionOfDefinition(_data._time) );
	}
	s << subDotEntry( "localMemory", _data._localInfos._memory );
	s << subDotEntry( "globalMemory", _data._globalInfos._memory );
	s << subDotEntry( "time", _data._time );

	os << "[" << std::endl;
	os << dotEntry( "type", "Node" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << ", " << std::endl;
	os << "]" << std::endl;
	return os;
}


std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime& v )
{
	os << v.getKey();
	return os;
}

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime::Key& v )
{
	os << "\"" << v.getName() << "\"" << " at time " << v.getTime();
	return os;
}

}
}
}
