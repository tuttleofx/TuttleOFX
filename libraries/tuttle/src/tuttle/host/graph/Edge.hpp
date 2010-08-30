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
	{
	}

	Edge( const std::string& out, const std::string& in, const std::string& inAttrName )
		: _out( out ),
		_in( in ),
		_inAttrName( inAttrName )
	{
	}

	//Edge( const Edge& e );

	virtual ~Edge()
	{
	}

	// operators
	Edge& operator=( const Edge& e )
	{
		if( this == &e )
			return *this;
		_in         = e.getIn();
		_out        = e.getOut();
		_inAttrName = e.getInAttrName();
		return *this;
	}

	const std::string& getOut() const        { return _out; }
	const std::string& getIn() const         { return _in; }
	const std::string& getInAttrName() const { return _inAttrName; }
	std::string getName() const { return std::string( getOut() ) + "." + kOfxOutputAttributeName + "-->" + getIn() + "." + getInAttrName(); }

	friend std::ostream& operator<<( std::ostream& os, const Edge& v );

private:
	std::string _out;
	std::string _in;
	std::string _inAttrName;
};

}
}
}

#endif

