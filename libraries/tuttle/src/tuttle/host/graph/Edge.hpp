#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxAttribute.h>

#include <iostream>
#include <string>

namespace tuttle {
namespace host {
namespace graph {

class Edge
{
public:
	Edge()
	{}

	Edge( const std::string& out, const std::string& in, const std::string& inAttrName )
		: _out( out )
		, _in( in )
		, _inAttrName( inAttrName )
		, _name( std::string( out ) + "." + kOfxOutputAttributeName + "-->" + in + "." + inAttrName )
	{}

	Edge( const Edge& e )
	{
		this->operator=( e );
		//_id = _count++;
	}

	virtual ~Edge()
	{}

	// operators
	Edge& operator=( const Edge& e )
	{
		if( this == &e )
			return *this;
		_in         = e._in;
		_out        = e._out;
		_inAttrName = e._inAttrName;
		_name       = e._name;
		return *this;
	}

	const std::string& getOut() const        { return _out; }
	const std::string& getIn() const         { return _in; }
	const std::string& getInAttrName() const { return _inAttrName; }
	const std::string& getName() const       { return _name; }

	friend std::ostream& operator<<( std::ostream& os, const Edge& v );

public:
	std::string _out;
	std::string _in;
	std::string _inAttrName;
	std::string _name;
};

}
}
}

#endif

