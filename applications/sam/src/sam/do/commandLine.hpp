#ifndef _SAM_DO_COMMANDLINE_HPP_
#define	_SAM_DO_COMMANDLINE_HPP_

#include "global.hpp"

#include <iterator>
#include <vector>
#include <string>

namespace sam {
namespace samdo {

#ifndef SAM_DO_PIPE_STR
#define SAM_DO_PIPE_STR "//"
#endif

static const std::string kpipe = SAM_DO_PIPE_STR;


/**
 * @brief Decomposes command line arguments into a list of options and a list of node command lines. Groups the arguments without insterpretation at this step.
 * 
 * @param[in]	argc	number of arguments ont the command line
 * @param[in]	argv	list of string arguments
 * @param[out]	cl_options	list of options for sam-do
 * @param[out]	cl_commands	list of node command lines (list of strings groups without insterpretation at this step)
 * @param[in]	pipe	pipe character
 */
void decomposeCommandLine( const int argc, char** const argv, std::vector<std::string>& cl_options, std::vector< std::vector<std::string> >& cl_commands, const std::string& pipe = kpipe )
{
	cl_commands.reserve(10);
	cl_commands.resize(1); // first node for options

	// split the command line to identify the multiple parts
	for( int i = 1; i < argc; ++i )
	{
		const std::string s( argv[i] );
		if( s == pipe )
		{
			cl_commands.resize( cl_commands.size()+1 );
			cl_commands.back().reserve(10);
		}
		else
		{
			cl_commands.back().push_back( s );
		}
	}

	// reoganize nodes
	// first and last nodes may only be flags to sam-do itself
	{
		// First node
		std::vector<std::string>& firstNode = cl_commands.front();
		std::vector<std::string>::iterator fNodeIt = firstNode.begin();
		std::vector<std::string>::iterator fNodeItEnd = firstNode.end();
		while( fNodeIt != fNodeItEnd &&
			   (*fNodeIt)[0] == '-' )
		{
			++fNodeIt;
		}
		// options in front of the first node are options for sam-do
		if( firstNode.begin() != fNodeIt )
		{
			std::copy( firstNode.begin(), fNodeIt, std::back_inserter(cl_options) );
			firstNode.erase( firstNode.begin(), fNodeIt );
		}
		if( firstNode.size() == 0 )
		{
			cl_commands.erase( cl_commands.begin() );
		}
	}
	if( cl_commands.size() )
	{
		// Last node
		// Is it a node or only flags?
		const std::vector<std::string>& notNode = cl_commands.back();
		if( notNode[0][0] == '-' )
		{
			std::copy( notNode.begin(), notNode.end(), std::back_inserter(cl_options) );
			cl_commands.erase( cl_commands.end()-1 );
		}
	}
}

}
}

#endif

