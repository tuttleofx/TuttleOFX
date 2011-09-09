#include <sam/common/color.hpp>

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

namespace sam
{
	Color _color;
}

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
	using namespace sam;

	EMaskType                 researchMask        = eMaskTypeSequence;  // by default show sequences
	EMaskOptions              descriptionMask     = eMaskOptionsNone;   // by default show nothing
	bool                      recursiveListing    = false;
	bool                      script              = false;
	bool                      enableColor         = false;
	std::string               availableExtensions;
	std::vector<std::string>  paths;
	std::vector<std::string>  filters;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("all,a"            , "do not ignore entries starting with .")
		("directories,d"    , "show directories in path(s)")
		("expression,e"     , bpo::value<std::string>(), "list with a specific pattern, ex: *.jpg,*.png")
		("files,f"          , "show files in path(s)")
		("help,h"           , "show this help")
		("long-listing,l"   , "use a long listing format")
		("mask,m"           , "mask sequences in path(s)")
		("relative-path,p"  , "show the root path for each objects")
		("recursive,R"      , "list subdirectories recursively")
		("absolute-path"    , "show the absolute path, not relative like path-root")
		("color"            , "color the output")
		("full-display"     , "show directories, files and sequences")
		("script"           , "output is formated to using in script files")
		("brief"            , "brief summary of the tool")
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		("input-dir", bpo::value< std::vector<std::string> >(), "input directories")
		("enable-color", bpo::value<std::string>(), "enable (or disable) color")
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add("input-dir", -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);
	
	bpo::variables_map vm;

	try
	{
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

		// get environment options and parse them
		if( const char* env_ls_options = std::getenv("SAM_LS_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_ls_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_COUT("sam-ls: command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_COUT("sam-ls: unknown error in command line.");
		exit( -2 );
	}

	if ( vm.count("script") )
	{
		// disable color, disable directory printing and set relative path by default
		script = true;
		descriptionMask |= eMaskOptionsAbsolutePath;
	}

	if ( vm.count("color") && !script )
	{
		enableColor = true;
	}
	if ( vm.count("enable-color") && !script )
	{
		std::string str = vm["enable-color"].as<std::string>();

		if( str == "1" || boost::iequals(str, "y") || boost::iequals(str, "Y") || boost::iequals(str, "yes") || boost::iequals(str, "Yes") || boost::iequals(str, "true") || boost::iequals(str, "True") )
		{
			enableColor = true;
		}
		else
		{
			enableColor = false;
		}
	}

	if( enableColor )
	{
		descriptionMask |= eMaskOptionsColor;
		_color.enable();
	}

	if (vm.count("help"))
	{
		TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-ls - list directory contents" << _color._std << std::endl);
		TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-ls [options] [directories]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std << std::endl );

		TUTTLE_COUT( "List information about the sequences, files and folders." );
		TUTTLE_COUT( "List the current directory by default, and only sequences." );
		TUTTLE_COUT( "The script option disable color, disable directory printing (in multi-directory case or recursive) and set relative path by default." << std::endl );

		TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std << std::endl );
		TUTTLE_COUT( mainOptions );
		return 0;
	}

	if ( vm.count("brief") )
	{
		TUTTLE_COUT( _color._green << "list directory contents" << _color._std);
		return 0;
	}

	if (vm.count("expression"))
	{
		TUTTLE_COUT( _color._red << "Expression: " << vm["expression"].as<std::string>() << _color._std );
		bal::split( filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}

	if (vm.count("directories"))
	{
		researchMask |= eMaskTypeDirectory;
	}
	
	if (vm.count("files"))
	{
		researchMask |= eMaskTypeFile;
	}
	
	if (vm.count("mask"))
	{
		researchMask &= ~eMaskTypeSequence;
	}
	
	if (vm.count("full-display"))
	{
		researchMask |= eMaskTypeDirectory;
		researchMask |= eMaskTypeFile;
		researchMask |= eMaskTypeSequence;
	}
	
	if (vm.count("all"))
	{
		// add .* files
		descriptionMask |= eMaskOptionsDotFile;
	}
	
	if (vm.count("long-listing"))
	{
		descriptionMask |= eMaskOptionsProperties;
	}
	
	if (vm.count("relative-path") )
	{
		descriptionMask |= eMaskOptionsPath;
	}

	if(vm.count("absolute-path"))
	{
		descriptionMask |= eMaskOptionsAbsolutePath;
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
		std::size_t index = 0;
		BOOST_FOREACH( bfs::path path, paths )
		{
<<<<<<< HEAD
			if( path == bfs::path(".") )
			{
				path = bfs::path("./");
			}
=======
			//TUTTLE_COUT( "path: "<< path );
>>>>>>> 6fa0f1abe0806f9834d74583cb6985782243fc01
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{

					if( ( paths.size() > 1 || recursiveListing ) && !script )
					{
						if( index > 0 )
						{
							TUTTLE_COUT( "" );
						}
						TUTTLE_COUT( path.string() << " :");
					}

					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( path, filters, researchMask, descriptionMask );
					BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
					{
					    TUTTLE_COUT( *s );
					}
					
					if(recursiveListing)
					{
						for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
						{
							if( bfs::is_directory( *dir ) )
							{
								bfs::path currentPath = (bfs::path)*dir;
								if( !script )
									TUTTLE_COUT( "\n" << currentPath.string() << " :" );

								std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( currentPath, filters, researchMask, descriptionMask );
								BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
								{
									TUTTLE_COUT( *s );
								}
							}
						}
					}

				}
				else
				{
					//TUTTLE_COUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path.branch_path(), filters, researchMask, descriptionMask );
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
					bfs::path basepath = path.branch_path();
					if( basepath == "" )
						basepath = "./";

					if( ( paths.size() > 1 || recursiveListing ) && !script )
					{
						if( index > 0 )
						{
							TUTTLE_COUT( "" );
						}
						TUTTLE_COUT( basepath.string() << " :");
					}

					Sequence s( basepath, descriptionMask );
					//TUTTLE_COUT ( basepath << "    @@@    " << path.string() );
					s.initFromDetection( path.string(), Sequence::ePatternDefault );
					s.setMaskOptions( descriptionMask );
					s.setMaskType   ( researchMask );
					s.setDirectory  ( basepath );

					if( s.getNbFiles() )
					{
						TUTTLE_COUT( s );
					}
					else
					{
						File f = File( basepath, path.leaf().string(), descriptionMask);
						TUTTLE_COUT( f );
					}
				}
				catch(... )
				{
					TUTTLE_CERR ( "Unrecognized pattern \"" << path << "\"" );
				}
			}
			++index;
		}
	}
	catch ( const bfs::filesystem_error& ex)
	{
		TUTTLE_COUT( ex.what() );
	}
	catch( ... )
	{
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}

	return 0;
}
