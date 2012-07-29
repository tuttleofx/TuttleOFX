#ifndef _TUTTLE_HOST_CORE_NODELISTARG_HPP_
#define _TUTTLE_HOST_CORE_NODELISTARG_HPP_

#include "INode.hpp"

#include <string>
#include <list>

namespace tuttle {
namespace host {

/**
 * @brief An utility class to use as function argument. It allows to create a list of nodes from multiple inputs.
 * 
 * All constructors are not "explicit", so we could automatically convert the
 * input inside when we use it as a function argument.
 */
class NodeListArg
{
public:
	NodeListArg()
	{}
	NodeListArg( const std::list<std::string>& nodes )
	: _nodes( nodes )
	{}
	NodeListArg( const std::list<INode*>& nodes );
	NodeListArg( const std::string& node )
	{
		_nodes.push_back( node );
	}
	NodeListArg( const INode& node )
	{
		_nodes.push_back( node.getName() );
	}

public:
	const std::list<std::string>& getNodes() const { return _nodes; }

private:
	std::list<std::string> _nodes;
};

}
}

#endif
