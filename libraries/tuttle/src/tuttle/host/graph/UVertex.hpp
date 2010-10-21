#ifndef _TUTTLE_PROCESSVERTEX_HPP_
#define _TUTTLE_PROCESSVERTEX_HPP_

#include <tuttle/host/INode.hpp>

#include <string>
#include <iostream>

namespace tuttle {
namespace host {
namespace graph {

class UVertex
{
public:
	UVertex()
	{}

	~UVertex() {}

//	const std::string& getName() const        { return _vertex.getName(); }
//	INode&             getProcessNode()       { return _vertex.getProcessNode(); }
//	const INode&       getProcessNode() const { return _vertex.getProcessNode(); }

	friend std::ostream& operator<<( std::ostream& os, const UVertex& v );
};

}
}
}

#endif

