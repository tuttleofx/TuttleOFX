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
		: _name( v._name ),
		_processNode( v._processNode )
	{}

	virtual ~Vertex()
	{}

	// operators
	Vertex& operator=( const Vertex& v )
	{
		if( this == &v )
			return *this;
		_name        = v._name;
		_processNode = v._processNode;
		return *this;
	}

	const std::string&             getName() const                        { return _name; }
	core::ProcessNode*             getProcessNode()                       { return _processNode; }
	const core::ProcessNode* const getProcessNode() const                 { return _processNode; }
	void                           setProcessNode( core::ProcessNode* p ) { _processNode = p; }
	core::ProcessOptions&          getProcessOptions() { return _processOptions; }
	const core::ProcessOptions&    getProcessOptions() const { return _processOptions; }
	void                           setProcessOptions( core::ProcessOptions& options ) { _processOptions = options; }

	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

private:
	std::string _name;
	core::ProcessNode* _processNode;
	core::ProcessOptions _processOptions;
};

}
}
}

#endif

