#include <sam/common/utility.hpp>
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

#include <Detector.hpp>

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

template<class T>
void coutVec( const boost::ptr_vector<T>& v )
{
	BOOST_FOREACH( const T& f, v )
	{
		std::cout << f << std::endl;
	}
}


int main( int argc, char** argv )
{
	using namespace tuttle::common;
	using namespace sam;

	sequenceParser::EMaskType                 researchMask        = sequenceParser::eMaskTypeSequence;  // by default show sequences
	sequenceParser::EMaskOptions              descriptionMask     = sequenceParser::eMaskOptionsNone;   // by default show nothing
	bool                                      recursiveListing    = false;
	bool                                      script              = false;
	bool                                      enableColor         = false;
	std::string                               availableExtensions;
	std::vector<std::string>                  paths;
	std::vector<std::string>                  filters;
	sequenceParser::Detector                  detector;

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
		if( const char* env_ls_options = std::getenv("SAM_OPTIONS") )
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
		descriptionMask |= sequenceParser::eMaskOptionsAbsolutePath;
	}

	if ( vm.count("color") && !script )
	{
		enableColor = true;
	}
	if ( vm.count("enable-color") && !script )
	{
		const std::string str = vm["enable-color"].as<std::string>();
		enableColor = string_to_boolean( str );
	}

	if( enableColor )
	{
		descriptionMask |= sequenceParser::eMaskOptionsColor;
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
		researchMask |= sequenceParser::eMaskTypeDirectory;
	}
	
	if (vm.count("files"))
	{
		researchMask |= sequenceParser::eMaskTypeFile;
	}
	
	if (vm.count("mask"))
	{
		researchMask &= ~sequenceParser::eMaskTypeSequence;
	}
	
	if (vm.count("full-display"))
	{
		researchMask |= sequenceParser::eMaskTypeDirectory;
		researchMask |= sequenceParser::eMaskTypeFile;
		researchMask |= sequenceParser::eMaskTypeSequence;
	}
	
	if (vm.count("all"))
	{
		// add .* files
		descriptionMask |= sequenceParser::eMaskOptionsDotFile;
	}
	
	if (vm.count("long-listing"))
	{
		descriptionMask |= sequenceParser::eMaskOptionsProperties;
	}
	
	if (vm.count("relative-path") )
	{
		descriptionMask |= sequenceParser::eMaskOptionsPath;
	}

	if(vm.count("absolute-path"))
	{
		descriptionMask |= sequenceParser::eMaskOptionsAbsolutePath;
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

	std::list<boost::shared_ptr<sequenceParser::FileObject> > listing;
	try
	{
		std::size_t index = 0;
		BOOST_FOREACH( bfs::path path, paths )
		{
			if( path == bfs::path(".") )
			{
				path = bfs::path("./");
			}
			if( ( paths.size() > 1 || recursiveListing ) && !script )
			{
				if( index > 0 )
				{
					TUTTLE_COUT( "" );
				}
				TUTTLE_COUT( path.string() << ":");
			}

			coutVec( detector.fileObjectInDirectory( path.string(), filters, researchMask, descriptionMask ) );

			if(recursiveListing)
			{
				for ( bfs::recursive_directory_iterator end, dir( path ); dir != end; ++dir )
				{
					if( bfs::is_directory( *dir ) )
					{
						bfs::path currentPath = (bfs::path)*dir;
						if( !script )
							TUTTLE_COUT( "\n" << currentPath.string() << ":" );

						coutVec( detector.fileObjectInDirectory( currentPath.string(), filters, researchMask, descriptionMask ) );
					}
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
