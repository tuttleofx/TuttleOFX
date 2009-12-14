#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include <iostream>

#include <tuttle/host/core/ProcessNode.hpp>

namespace tuttle {
namespace host {
namespace graph {

class Vertex
{
public:
	Vertex()
		: _name( "Undefined" ),
		_processNode( NULL )
	{}

	Vertex( const std::string& name,
	        core::ProcessNode& processNode )
		: _name( name ),
		_processNode( &processNode )
	{}

	Vertex( Vertex& v )
		: _name( v.name() ),
		_processNode( v.processNode() ) {}

	virtual ~Vertex()
	{}

	// operators
	Vertex& operator=( const Vertex& v )
	{
		if( this == &v )
			return *this;
		_name        = v.name();
		_processNode = v.processNode();
		return *this;
	}

	const std::string& name() const                           { return _name; }
	core::ProcessNode* processNode() const                    { return _processNode; }
	void               setProcessNode( core::ProcessNode* p ) { _processNode = p; }

	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

private:
	std::string _name;
	core::ProcessNode* _processNode;
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

