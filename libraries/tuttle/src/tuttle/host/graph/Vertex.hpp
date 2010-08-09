#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include <iostream>

#include <tuttle/host/Node.hpp>

namespace tuttle {
namespace host {
namespace graph {

class Vertex
{
public:
	Vertex( const std::string& name = "Undefined" )
		: _name( name ),
		_processNode( NULL ),
		_fake( true )
	{}

	Vertex( const std::string& name,
	        Node& processNode )
		: _name( name ),
		_processNode( &processNode ),
		_fake( false )
	{}

	Vertex( Vertex& v )
	{
		this->operator=(v);
	}

	virtual ~Vertex()
	{}

	// operators
	Vertex& operator=( const Vertex& v )
	{
		if( this == &v )
			return *this;
		_name        = v._name;
		_processNode = v._processNode;
		_fake = v._fake;
		return *this;
	}

	const bool                     isFake() const                         { return _fake; }
	const std::string&             getName() const                        { return _name; }
	Node*             getProcessNode()                       { return _processNode; }
	const Node* const getProcessNode() const                 { return _processNode; }
	void                           setProcessNode( Node* p ) { _processNode = p; }
	ProcessOptions&          getProcessOptions() { return _processOptions; }
	const ProcessOptions&    getProcessOptions() const { return _processOptions; }
	void                           setProcessOptions( ProcessOptions& options ) { _processOptions = options; }

	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

private:
	std::string _name;
	Node* _processNode;
	graph::ProcessOptions _processOptions;
	bool _fake;
};

}
}
}

#endif

