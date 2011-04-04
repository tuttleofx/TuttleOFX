#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
	return os;
}

int main( int argc, char** argv )
{
	using namespace tuttle::common;

	MaskType				researchMask		= eSequence;	// by default show sequences
	MaskOptions				descriptionMask		= eNone;	// by default show nothing
	bool					recursiveListing	= false;
	std::string				availableExtensions;
	std::vector<std::string>		paths;
	std::vector<std::string>		filters;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("all,a"		, "do not ignore entries starting with .")
		("directories,d"	, "show directories in path(s)")
		("expression,e"		, bpo::value<std::string>(), "list with a specific pattern, ex: *.jpg,*.png")
		("files,f"		, "show files in path(s)")
		("help,h"		, "show this help")
		("long-listing,l"	, "use a long listing format")
		("mask,m"		, "mask sequences in path(s)")
		("path-root,p"		, "show the root path for each objects")
		("recursive,R"		, "list subdirectories recursively")
		("absolute-path"        , "show the absolute path, not relative like path-root")
		("color"		, "color the output")
		("full-display"		, "show directories, files and sequences")
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		("input-dir", bpo::value< std::vector<std::string> >(), "input directories")
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add("input-dir", -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);

	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

	// get environnement options and parse them
	if( std::getenv("SAM_LS_OPTIONS") != NULL)
	{
		std::vector<std::string> envOptions;
		std::string env = std::getenv("SAM_LS_OPTIONS");
		envOptions.push_back( env );
		bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
	}

	bpo::notify(vm);    

	if (vm.count("help"))
	{
		TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
		TUTTLE_COUT( "NAME");
		TUTTLE_COUT( "\tsam-ls - list directory contents\n" );
		TUTTLE_COUT( "SYNOPSIS\n\tsam-ls [options] [directories]\n" );
		TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
		return 1;
	}

	if (vm.count("extension"))
	{
		bal::split( filters, vm["extension"].as<std::string>(), bal::is_any_of(","));
	}

	if (vm.count("directories"))
	{
		researchMask |= eDirectory;
	}
	
	if (vm.count("files"))
	{
		researchMask |= eFile;
	}
	
	if (vm.count("mask"))
	{
		researchMask &= ~eSequence;
	}
	
	if (vm.count("full-display"))
	{
		researchMask |= eDirectory;
		researchMask |= eFile;
		researchMask |= eSequence;
	}
	
	if (vm.count("all"))
	{
		// add .* files
		descriptionMask |= eDotFile;
	}
	
	if (vm.count("long-listing"))
	{
		descriptionMask |= eProperties;
	}
	
	if (vm.count("path-root"))
	{
		descriptionMask |= ePath;
	}

	if(vm.count("absolute-path"))
	{
		descriptionMask |= eAbsolutePath;
	}

	if (vm.count("color"))
	{
		descriptionMask |= eColor;
	}
	
	// defines paths, but if no directory specify in command line, we add the current path
	if (vm.count("input-dir"))
	{
		paths = vm["input-dir"].as< std::vector<std::string> >();
	}
	else
	{
		paths.push_back( "./" );
	}
	
	if (vm.count("recursive"))
	{
		recursiveListing = true;
	}


// 	for(uint i=0; i<filters.size(); i++)
// 	  TUTTLE_COUT("filters = " << filters.at(i));
// 	TUTTLE_COUT("research mask = " << researchMask);
// 	TUTTLE_COUT("options  mask = " << descriptionMask);

	try
	{
		BOOST_FOREACH( bfs::path path, paths )
		{
//			TUTTLE_COUT( "path: "<< path );
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{
//					TUTTLE_COUT( "is a directory" );
					if(recursiveListing)
					{
						for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
						{
							if( bfs::is_directory( *dir ) )
							{
//								TUTTLE_COUT( *dir );
								std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)*dir, researchMask, descriptionMask, filters );
								BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
								{
									TUTTLE_COUT( *s );
								}
							}
						}
					}

					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path, researchMask, descriptionMask, filters );
					BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
					{
					    TUTTLE_COUT( *s );
					}

				}
				else
				{
//					TUTTLE_COUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path.branch_path(), researchMask, descriptionMask, filters );
					BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
					{
						TUTTLE_COUT( *s );
					}
				}
			}
			else
			{
				//TUTTLE_COUT( "not exist ...." );
				try
				{
					Sequence s(path.branch_path(), descriptionMask );
					s.initFromDetection( path.string(), Sequence::ePatternDefault );
					if( s.getNbFiles() )
					{
						TUTTLE_COUT( s );
					}
				}
				catch(... )
				{
					TUTTLE_CERR ( "Unrecognized pattern \"" << path << "\"" );
				}
			}
		}
	}
	catch (bfs::filesystem_error &ex)
	{
		TUTTLE_COUT( ex.what() );
	}
	catch(... )
	{
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}

	return 0;
}

