#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class Edge
{
public:
	Edge() {}

	Edge( const std::string& name )
		: _name( name ) {}

	Edge( const Edge& e )
		: _name( e.name() ) {}

	virtual ~Edge()
	{}

	// operators
	Edge& operator=( const Edge& e )
	{
		if( this == &e )
			return *this;
		_name = e.name();
		return *this;
	}

	const std::string& name() const                   { return _name; }
	void               setName( const std::string s ) { _name = s; }

	friend std::ostream& operator<<( std::ostream& os, const Edge& v );

private:
	std::string _name;
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

