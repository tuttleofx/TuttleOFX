#ifndef _SAM_DO_NODE_GLOBAL_HPP_
#define	_SAM_DO_NODE_GLOBAL_HPP_

#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>

#include <tuttle/host/Graph.hpp>

#include <vector>
#include <string>

namespace sam {
namespace samdo {

namespace ttl = tuttle::host;

/*
struct NodeCommand
{
	NodeCommand( const std::vector<std::string>& commandLine )
	: _name( commandLine[0] )
	{
		BOOST_ASSERT( commandLine.size() );
	}

	std::string _name;
	std::vector< std::pair<std::string, std::string> > _params;
	std::vector< std::pair<std::string, std::string> > _flags;
};
*/

}
}

#endif


