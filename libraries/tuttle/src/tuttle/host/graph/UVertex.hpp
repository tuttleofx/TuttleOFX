#ifndef _TUTTLE_HOST_VERTEX_HPP_
#define _TUTTLE_HOST_VERTEX_HPP_

#include "IVertex.hpp"

#include <tuttle/host/INode.hpp>

#include <string>
#include <iostream>

namespace tuttle {
namespace host {
namespace graph {

class UVertex : public IVertex
{
public:
	typedef std::string Key;
public:
	UVertex()
	{}
	
	UVertex( const std::string& name, INode& processNode );

	~UVertex() {}

	friend std::ostream& operator<<( std::ostream& os, const UVertex& v );
};

}
}
}

#endif

