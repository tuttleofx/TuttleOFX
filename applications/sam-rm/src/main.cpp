#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>

#define STD_COLOR  "\E[0m"
#define FOLDER_COLOR "\E[1;34m"
#define FILE_COLOR "\E[0;32m"
#define SEQUENCE_COLOR "\E[0;32m"
#define MISSING_FILE_IN_SEQUENCE_COLOR "\E[31m"
#define ERROR_WHEN_DELETING "\E[31m"

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace ttl = tuttle::common;

bool colorOutput = true;
bool verbose     = false;

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
    return os;
}

void removeSequence( const ttl::Sequence& s )
{
	for( ttl::Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt(t);
		if( !bfs::exists( sFile ) )
		{
			if(colorOutput)
			{
			  TUTTLE_CERR("Could not remove: " << ERROR_WHEN_DELETING << sFile.string() << STD_COLOR);
			}
			else
			{
			  TUTTLE_CERR("Could not remove: " << sFile.string() );
			}
		}
		else
		{
			if(verbose)
			{
			  if(colorOutput)
			  {
			    TUTTLE_COUT("remove: " << FOLDER_COLOR << sFile.string() << STD_COLOR );
			  }
			  else
			  {
			    TUTTLE_COUT("remove: " << sFile.string() );
			  }
			}
			bfs::remove( sFile );
		}
	}
}

void removeFileObject( std::list<boost::shared_ptr<ttl::FileObject> > &listing, std::vector<boost::filesystem::path> &notRemoved )
{
	BOOST_FOREACH( const std::list<boost::shared_ptr<ttl::FileObject> >::value_type & s, listing )
	{
		if( !(s->getMaskType () == ttl::eDirectory))
		{
			if(verbose)
			  TUTTLE_COUT( "remove " << *s );
			std::vector<bfs::path> paths = s->getFiles();
			for(uint i=0; i<paths.size(); i++)
				bfs::remove(paths.at(i));
		}
		else // is a directory
		{
			std::vector<boost::filesystem::path> paths = s->getFiles();
			for(uint i=0; i<paths.size(); i++)
			{
				if(bfs::is_empty(paths.at(i)))
				{
					if(verbose)
					  TUTTLE_COUT( "remove " << *s );
					bfs::remove(paths.at(i));
				}
				else
				{
					notRemoved.push_back( paths.at(i) );
				}
			}
			
		}
	}
}

void removeFileObject( std::vector<boost::filesystem::path> &listing )
{
	std::sort(listing.begin(), listing.end());
	std::reverse(listing.begin(), listing.end());
	BOOST_FOREACH( const boost::filesystem::path paths, listing )
	{
		if(bfs::is_empty(paths))
		{
			if(verbose)
			{
			  if(colorOutput)
			  {
			    TUTTLE_COUT( "remove " << FOLDER_COLOR << paths << STD_COLOR );
			  }
			  else
			  {
			    TUTTLE_COUT( "remove " << paths );
			  }
			}
			bfs::remove(paths);
		}
		else
		{
			if(colorOutput)
			{
			  TUTTLE_CERR( "could not remove " << ERROR_WHEN_DELETING << paths << STD_COLOR );
			}
			else
			{
			  TUTTLE_CERR( "could not remove " << paths );
			}
			
		}
	}
}

int main( int argc, char** argv )
{
	using namespace tuttle::common;

	int					researchMask		= eSequence;	// by default show sequences
	MaskOptions				descriptionMask		= eColor;	// by default show nothing
	bool					recursiveListing	= false;
	std::string				availableExtensions;
	std::vector<std::string>		paths;
	std::vector<std::string>		filters;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("all,a"		, "do not ignore entries starting with .")
		("directories,d"	, "remove directories in path(s)")
		("extension,e"		, bpo::value<std::string>(), "remove with a specific pattern, ex: *.jpg,*.png")
		("files,f"		, "remove files in path(s)")
		("help,h"		, "show this help")
		("mask,m"		, "not remove sequences in path(s)")
		("path-root,p"		, "show the root path for each objects")
		("recursive,R"		, "remove subdirectories recursively")
		("verbose,v"		, "explain what is being done")
		("no-color"		, "no color on the outup")
		("full-rm"		, "remove directories, files and sequences")
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

	bpo::notify(vm);    

	if (vm.count("help"))
	{
	    TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
	    TUTTLE_COUT( "NAME\n\tsam-rm - remove directory contents\n" );
	    TUTTLE_COUT( "SYNOPSIS\n\tsam-rm [options] [directories]\n" );
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
	
	if (vm.count("verbose"))
	{
	    verbose = true;
	}
	
	if (vm.count("no-color"))
	{
	    colorOutput = false;
	    remove( descriptionMask, eColor );
	}
	
	if (vm.count("full-rm"))
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
	
	if (vm.count("path-root"))
	{
	    descriptionMask |= ePath;
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
	    std::vector<boost::filesystem::path> pathsNoRemoved;
	    BOOST_FOREACH( bfs::path path, paths )
	    {
// 		TUTTLE_COUT( "path: "<< path );
		if( bfs::exists( path ) )
		{
		    if( bfs::is_directory( path ) )
		    {
// 			TUTTLE_COUT( "is a directory" );
			if(recursiveListing)
			{
			    for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
			    {
				if( bfs::is_directory( *dir ) )
				{
// 				    TUTTLE_COUT( *dir );
				    std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)*dir, researchMask, descriptionMask, filters );
				    removeFileObject( listing, pathsNoRemoved );
				}
			    }
			}
			else
			{
			    std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path, researchMask, descriptionMask, filters );
			    removeFileObject( listing, pathsNoRemoved );
			}
		    }
		    else
		    {
// 			TUTTLE_COUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
			filters.push_back( path.leaf().string() );
			std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path.branch_path(), researchMask, descriptionMask, filters );
			removeFileObject( listing, pathsNoRemoved );
		    }
		}
		else
		{
// 		    TUTTLE_COUT( "not exist ...." );
		    try
		    {
			Sequence s( path, descriptionMask );
			s.initFromDetection();
			if( s.getNbFiles() )
			{
			    TUTTLE_COUT(s);
			    removeSequence( s );
			}
		    }
		    catch(... )
		    {
			TUTTLE_CERR ( "Unrecognized pattern \"" << path << "\"" );
		    }
		}
	    }
	    // delete not empty folder the first time
	    removeFileObject( pathsNoRemoved );
	}
	catch(... )
	{
	    TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}

	return 0;
}

