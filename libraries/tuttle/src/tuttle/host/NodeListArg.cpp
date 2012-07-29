#include "NodeListArg.hpp"

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {

NodeListArg::NodeListArg( const std::list<INode*>& nodes )
{
	BOOST_FOREACH( INode * n, nodes )
	{
		_nodes.push_back( n->getName() );
	}
}

}
}
