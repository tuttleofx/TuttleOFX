#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include <iostream>

#include <tuttle/host/Node.hpp>
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace host {
namespace graph {

class Vertex
{
public:
	Vertex( const std::string& name = "Undefined" )
		: _name( name ),
		_processNode( NULL ),
		_fake( true ),
		_used( true ),
		_id( _count++ )
	{
	}

	Vertex( const std::string& name,
	        Node& processNode )
		: _name( name ),
		_processNode( &processNode ),
		_fake( false ),
		_used( true ),
		_id( _count++ )
	{
	}

	Vertex( Vertex& v )
	{
		this->operator=(v);
		_id = _count++;
	}

	virtual ~Vertex()
	{
	}

	Vertex& operator=( const Vertex& v )
	{
		if( this == &v )
			return *this;
		_name        = v._name;
		_processNode = v._processNode;
		_processOptions = v._processOptions;
		_fake = v._fake;
		_used = v._used;
		return *this;
	}

	bool isFake() const { return _fake; }
	void setUsed( const bool used = true ) { _used = used; }
	bool isUsed() const { return _used; }
	const std::string& getName() const                        { return _name; }
	Node*             getProcessNode()                       { return _processNode; }
	const Node* const getProcessNode() const                 { return _processNode; }
	void                           setProcessNode( Node* p ) { _processNode = p; }
	ProcessOptions&          getProcessOptions() { return _processOptions; }
	const ProcessOptions&    getProcessOptions() const { return _processOptions; }
	void                           setProcessOptions( const ProcessOptions& options ) { _processOptions = options; }

	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

private:
	std::string _name;
	Node* _processNode;
	graph::ProcessOptions _processOptions;
	bool _fake;
	bool _used;
	static int _count;
public:
	int _id;
};

}
}
}

#endif

