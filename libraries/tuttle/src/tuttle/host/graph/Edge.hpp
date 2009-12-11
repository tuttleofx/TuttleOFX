#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include <string>
#include <ofxAttribute.h>

namespace tuttle {
namespace host {
namespace graph {

class Edge
{
public:
	Edge() {}

	Edge( const std::string& out, const std::string& in, const std::string& inAttrName )
	:_out(out)
	,_in(in)
	,_inAttrName(inAttrName)
	{
	}

	//Edge( const Edge& e );

	virtual ~Edge()
	{}

	// operators
	Edge& operator=( const Edge& e )
	{
		if( this == &e )
			return *this;
		_in = e.in();
		_out = e.out();
		_inAttrName = e.inAttrName();
		return *this;
	}

	const std::string& out() const  { return _out; }
	const std::string& in() const  { return _in; }
	const std::string& inAttrName() const  { return _inAttrName; }
	const std::string name() const  { return out() + "." + kOfxOutputAttributeName + "-->" + in() + "." + inAttrName(); }

	friend std::ostream& operator<<( std::ostream& os, const Edge& v );

private:
	std::string _out;
	std::string _in;
	std::string _inAttrName;
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

