#include <tuttle/common/clip/Sequence.hpp>

#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>

#include <algorithm>
#include <iostream>
#include <bits/stl_vector.h>

#include "tuttle/common/exceptions.hpp"
#include "tuttle/host/Graph.hpp"

#ifndef SAMDO_PIPE_STR
#define SAMDO_PIPE_STR "//"
#endif

static const std::string kpipe = SAMDO_PIPE_STR;

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

int main( int argc, char** argv )
{
	try
	{
		if( argc <= 1 ) // no argument
		{
			TUTTLE_COUT(
				"sam-do: missing operands.\n"
				"'sam-do --help' for more informations.\n"
			);
			exit( -1 );
		}
		
		std::vector<std::string> cl_options;
		std::vector< std::vector<std::string> > cl_commands;
		cl_commands.reserve(10);
		cl_commands.resize(1); // first node for options

		// split the command line to identify the multiple parts
		for( int i = 1; i < argc; ++i )
		{
			std::string s( argv[i] );
			if( s == kpipe )
			{
				cl_commands.resize( cl_commands.size()+1 );
				cl_commands.back().reserve(10);
				continue;
			}
			cl_commands.back().push_back( s );
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

		namespace ttl = tuttle::host;

		ttl::Core::instance().preload();

		// create the graph
		ttl::Graph graph;
		std::list<ttl::Graph::Node*> nodes;
		// Analyse each part of the command line
		{
			namespace bpo = boost::program_options;
			// Analyse sam-do flags
			try
			{
				// Declare the supported options.
				bpo::options_description infoOptions( "\tDisplay options" );
				infoOptions.add_options()
					("help,h"       , "show node help")
					("version,v"    , "display node version")
					("verbose,V"    , "explain what is being done")
					("nodes,n"      , "show list of all available nodes")
				;
				bpo::options_description confOptions( "\tConfigure process" );
				confOptions.add_options()
					("nb-cores"     , bpo::value<std::string>(), "set a fix number of CPUs")
				;

				bpo::options_description all_options;
				all_options.add(infoOptions).add(confOptions);

				bpo::variables_map samdo_vm;
				bpo::store( bpo::command_line_parser(cl_options).options(all_options).run(), samdo_vm );
				if( const char* env_ptr = std::getenv("SAM_DO_OPTIONS") )
				{
					std::vector<std::string> envOptions;
					std::string env( env_ptr );
					envOptions.push_back( env );
					bpo::store( bpo::command_line_parser(envOptions).options(all_options).run(), samdo_vm );
				}

				bpo::notify( samdo_vm );

				if( samdo_vm.count("help") )
				{
					TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]" );
					TUTTLE_COUT( "" );
					TUTTLE_COUT( "NAME" );
					TUTTLE_COUT( "\tsam-do - A command line to execute a list of OpenFX nodes." );
					TUTTLE_COUT( "\t         Use the sperarator // to pipe images between nodes." );
					TUTTLE_COUT( "" );
					TUTTLE_COUT( "SYNOPSIS" );
					TUTTLE_COUT( "\tsam-do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]" );
					TUTTLE_COUT( "" );
					TUTTLE_COUT( "EXAMPLE" );
					TUTTLE_COUT( "\tsam-do --verbose read toto.dpx // blur 3 // resize scale=50% // write toto.jpg" );
					TUTTLE_COUT( "" );
					TUTTLE_COUT( "DESCRIPTION" );
					TUTTLE_COUT( infoOptions );
					TUTTLE_COUT( confOptions );
					exit( 0 );
				}
			}
			catch( const boost::program_options::error& e )
			{
				TUTTLE_CERR( "sam-do options" );
				TUTTLE_CERR( "Error: " << e.what() );
				exit( -2 );
			}
			catch( ... )
			{
				TUTTLE_CERR( "sam-do options" );
				TUTTLE_CERR( "Error: " << boost::current_exception_diagnostic_information() );
				exit( -2 );
			}

			/// @todo Set all sam-do options for rendering

			// Analyse options for each node
			{
				// Declare the supported options.
				bpo::options_description infoOptions( "\tDisplay options" );
				infoOptions.add_options()
					("help,h"       , "show node help")
					("version,v"    , "display node version")
					("attributes,a" , "show all attributes: parameters+clips")
					("properties,p" , "list properties of the node")
					("clips,c"      , "list clips of the node")
					("clip"         , bpo::value<std::string>(), "display clip informations")
					("parameters,P" , "list parameters of the node")
					("param"        , bpo::value<std::string>(), "display parameter informations")
				;
				bpo::options_description confOptions( "\tConfigure process" );
				confOptions.add_options()
					("verbose,V"    , "explain what is being done")
					("nb-cores"     , bpo::value<std::string>(), "set a fix number of CPUs")
				;
				// describe hidden options
				bpo::options_description hiddenOptions;
				hiddenOptions.add_options()
					("param-values", bpo::value< std::vector<std::string> >(), "node parameters")
				;

				// define default options
				bpo::positional_options_description param_values;
				param_values.add("param-values", -1);

				bpo::options_description all_options;
				all_options.add(infoOptions).add(confOptions).add(hiddenOptions);

				BOOST_FOREACH( const std::vector<std::string>& command, cl_commands )
				{
					const std::string& nodeName = command[0];
					std::vector<std::string> nodeArgs;
					std::copy( command.begin()+1, command.end(), std::back_inserter(nodeArgs) );
					
					try
					{
						// parse the command line, and put the result in node_vm
						bpo::variables_map node_vm;
						bpo::store( bpo::command_line_parser(nodeArgs).options(all_options).positional(param_values).run(), node_vm );

						bpo::notify( node_vm );

						// Check priority flags:
						// If one flag to display informations is used in command line,
						// it replaces all the process.
						// --help,h --version,v --verbose,V --params --clips --props
						if( node_vm.count("help") )
						{
							TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]" );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "NODE" );
							TUTTLE_COUT( "\tsam-do " << nodeName << " - OpenFX node." );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "DESCRIPTION" );
							TUTTLE_COUT( "\tnode help !" ); ///< @todo node help !
							TUTTLE_COUT( "" );
							TUTTLE_COUT( infoOptions );
							TUTTLE_COUT( confOptions );
							exit(0);
						}
						if( node_vm.count("version") )
						{
							TUTTLE_COUT( "version " << "0.0" /* fetch node version */ );
							exit(0);
						}
						if( node_vm.count("attributes") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "ATTRIBUTES" );
							TUTTLE_COUT( "\tCLIPS" );
							/// @todo
							TUTTLE_COUT( "\tPARAMETERS" );
							/// @todo
							exit(0);
						}
						if( node_vm.count("properties") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "PROPERTIES" );
							/// @todo
							exit(0);
						}
						if( node_vm.count("clips") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "CLIPS" );
							/// @todo
							exit(0);
						}
						if( node_vm.count("clip") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "CLIP: " << node_vm["clip"].as<std::string>() );
							/// @todo
							exit(0);
						}
						if( node_vm.count("parameters") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "PARAMETERS" );
							/// @todo
							exit(0);
						}
						if( node_vm.count("param") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeName );
							TUTTLE_COUT( "PARAM: " << node_vm["clip"].as<std::string>() );
							/// @todo
							exit(0);
						}

						TUTTLE_COUT( "[" << nodeName << "]" );

						ttl::Graph::Node& currentNode = graph.createNode( "fr.tuttle." + nodeName );
						nodes.push_back( &currentNode );

						// Analyse parameters
						static const boost::regex re_param( "(?:([a-zA-Z_][a-zA-Z0-9_]*)=)?(.*)" );
						if( node_vm.count("param-values") )
						{
							bool orderedParams = true;
							std::size_t paramIdx = 0;
							const std::vector<std::string> params = node_vm["param-values"].as< std::vector<std::string> >();
							BOOST_FOREACH( const std::string& p, params )
							{
								boost::cmatch matches;
								if( ! boost::regex_match( p.c_str(), matches, re_param ) )
								{
									BOOST_THROW_EXCEPTION( tuttle::exception::Value()
										<< tuttle::exception::user() + "Parameter can't be parsed \"" + p + "\"." );
								}
								if( matches.size() != 3 )
								{
									// should never happen
									BOOST_THROW_EXCEPTION( tuttle::exception::Value()
										<< tuttle::exception::user() + "Parameter can't be parsed \"" + p + "\". " + matches.size() + " matches." );
								}
								const std::string paramName = matches[1];
								const std::string paramValue = matches[2];
								if( paramName.size() )
								{
									orderedParams = false;
								}
								else if( orderedParams == false )
								{
									BOOST_THROW_EXCEPTION( tuttle::exception::Value()
										<< tuttle::exception::user() + "Non-keyword parameter after keyword parameter. \"" + p + "\"." );
								}
//								TUTTLE_COUT( "* " << p );
//								TUTTLE_COUT( "3: " << paramName << " => " << paramValue );

								/// @todo setup the node with parameter value in tuttle.
								if( paramName.size() )
								{
									currentNode.getParam( paramName ).set( paramValue );
								}
								else
								{
									//currentNode.getParamSet().getParamList()[paramIdx].set( paramValue );
								}
								++paramIdx;
							}
						}
					}
					catch( const tuttle::exception::Common& e )
					{
						TUTTLE_CERR( "sam-do " << nodeName );
						TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) );
						exit( -2 );
					}
					catch( const boost::program_options::error& e )
					{
						TUTTLE_CERR( "sam-do " << nodeName );
						TUTTLE_CERR( "Error: " << e.what() );
						exit( -2 );
					}
					catch( ... )
					{
						TUTTLE_CERR( "sam-do " << nodeName );
						TUTTLE_CERR( "Error: " << boost::current_exception_diagnostic_information() );
						exit( -2 );
					}
				}
			}
		}

		// display nodes
//		BOOST_FOREACH( const std::string& option, cl_options )
//		{
//			TUTTLE_COUT( "| " << option );
//		}
//		BOOST_FOREACH( const std::vector<std::string>& node, cl_commands )
//		{
//			TUTTLE_COUT( "[" << node[0] << "]" );
//			for( std::size_t i = 1; i < node.size(); ++i )
//			{
//				const std::string& s = node[i];
//				if( s[0] == '-' )
//				{
//					TUTTLE_COUT( s );
//				}
//				else
//				{
//					TUTTLE_COUT( "* " << s );
//				}
//			}
//		}

		// Connect all nodes linearly
		graph.connect( nodes );

		// Execute the graph
		graph.compute( *nodes.back(), 0 );
	}
	catch( const tuttle::exception::Common& e )
	{
		TUTTLE_CERR( "sam-do" );
		TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) );
		exit( -2 );
	}
	catch( const boost::program_options::error& e )
	{
		TUTTLE_CERR( "sam-do");
		TUTTLE_CERR( "Error: " << e.what() );
		exit( -2 );
	}
	catch( ... )
	{
		TUTTLE_CERR( "sam-do");
		TUTTLE_CERR( boost::current_exception_diagnostic_information() );
		exit( -2 );
	}
	return 0;
}

