#include <sam/common/color.hpp>

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/common/exceptions.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>

#ifndef SAM_MOVEFILES
#define SAM_MV_OR_CP_OPTIONS	"SAM_CP_OPTIONS"
#define SAM_TOOL               "sam-cp"
#else
#define SAM_MV_OR_CP_OPTIONS	"SAM_MV_OPTIONS"
#define SAM_TOOL               "sam-mv"
#endif


namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bal = boost::algorithm;
namespace ttl = tuttle::common;

std::string  sColorStd;
std::string  sColorBlue;
std::string  sColorGreen;
std::string  sColorRed;
std::string  sColorError;
std::string  sColorFolder;

bool        colorOutput   = false;

void copy_sequence( const ttl::Sequence& s, const ttl::Sequence::Time firstImage, const ttl::Sequence::Time lastImage,
					const ttl::Sequence& d, int offset = 0 )
{
	// accept negative values in sequence
	//	if( ( offset <0 ) & ( s.getFirstTime()+offset <0 ) )
	//	{
	//		TUTTLE_COUT("ERROR: index could not be negative.");
	//		return ;
	//	}

	//	TUTTLE_COUT( firstImage << " | " << lastImage );
	//	if(s.getLastTime()-lastImage <= s.getFirstTime()+firstImage )
	//	{
	//		TUTTLE_COUT("error in index of first-image and/or last image");
	//		return;
	//	}
	ttl::Sequence::Time begin;
	ttl::Sequence::Time end;
	ttl::Sequence::Time step;
	if( offset > 0 )
	{
		begin = lastImage;
		end = firstImage;
		step = -s.getStep();
	}
	else
	{
		begin = firstImage;
		end = lastImage;
		step = s.getStep();
	}

	for( ttl::Sequence::Time t = begin; (offset > 0) ? (t >= end) : (t <= end); t += step )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt( t );
		//TUTTLE_TCOUT_VAR( sFile );
		if( bfs::exists( sFile ) )
		{
			bfs::path dFile = d.getAbsoluteFilenameAt( t + offset );
			//TUTTLE_TCOUT( "do " << sFile << " -> " << dFile );
#ifndef SAM_SAM_MOVEFILESS // copy file(s)
			if( bfs::exists( dFile ) )
			{
				TUTTLE_CERR( sColorError << "Could not copy: " << dFile.string( ) << sColorStd );
			}
			else
			{
				try
				{
					//TUTTLE_COUT( "copy " << sFile << " -> " << dFile );
					bfs::copy_file( sFile, dFile );
				}
				catch( const bpo::error& e )
				{
					TUTTLE_CERR( sColorError << "error : " << e.what() << sColorStd );
				}
				catch( ... )
				{
					TUTTLE_CERR( sColorError << boost::current_exception_diagnostic_information( ) << sColorStd );
				}
			}
#else // move file(s)
			if( bfs::exists( dFile ) )
			{
				TUTTLE_CERR( sColorError << "Could not move: " << dFile.string( ) << sColorStd );
			}
			else
			{
				try
				{
					//TUTTLE_COUT( "move " << sFile << " -> " << dFile );
					bfs::rename( sFile, dFile );
				}
				catch( const bpo::error& e )
				{
					TUTTLE_CERR( sColorError << "error : " << e.what() << sColorStd );
				}
				catch( ... )
				{
					TUTTLE_CERR( sColorError << boost::current_exception_diagnostic_information( ) << sColorStd );
				}
			}
#endif
		}
	}
}

void copy_sequence( const ttl::Sequence& s,
					const ttl::Sequence& d, const ttl::Sequence::Time offset = 0 )
{
	copy_sequence( s, s.getFirstTime(), s.getLastTime(),
				   d, offset );
}

void copy_sequence( const ttl::Sequence& s, const ttl::Sequence::Time firstImage, const ttl::Sequence::Time lastImage,
					const bfs::path& d, const ttl::Sequence::Time offset = 0 )
{
	ttl::Sequence dSeq(s); // create dst from src
	dSeq.setDirectory( d ); // modify path
	copy_sequence( s, firstImage, lastImage,
				   dSeq, offset );
}

void copy_sequence( const ttl::Sequence& s,
					const bfs::path& d, const ttl::Sequence::Time offset = 0 )
{
	copy_sequence( s, s.getFirstTime(), s.getLastTime(),
				   d, offset );
}

int sammvcp( int argc, char** argv )
{
	ttl::EMaskOptions descriptionMask      = ttl::eMaskOptionsNone; // by default show nothing
	std::string              availableExtensions;
	std::vector<std::string> paths;
	std::vector<std::string> filters;
	std::string              outputPattern;
	bool                     verbose       = false;
	bool                     dstIsSeq      = false;
	std::ssize_t             offset        = 0;
	bool                     hasInputFirst = false;
	std::ssize_t             inputFirst    = 0;
	bool                     hasInputLast  = false;
	std::ssize_t             inputLast     = 0;
	std::ssize_t             outputFirst   = 0;
	std::ssize_t             outputLast    = 0;
	typedef enum {
		eOffsetModeNotSet,
		eOffsetModeValue,
		eOffsetModeFirstTime,
		eOffsetModeLastTime,
	} EOffsetMode;
	EOffsetMode offsetMode = eOffsetModeNotSet;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options( )
		( "help,h"      , "show this help" )
		( "offset,o"    , bpo::value<std::ssize_t>( ), "retime the sequence with the given offset. ex: -o 1, -o \"-10\"" )
//		( "force,f"     , bpo::value<bool>( )        , "if a destination file exists, replace it" )
		( "verbose,v"   , "explain what is being done" )
		( "input-first" , bpo::value<std::ssize_t>( ), "specify the first input image, in order to select a sub-range of the input sequence" )
		( "input-last"  , bpo::value<std::ssize_t>( ), "specify the last input image, in order to select a sub-range of the input sequence" )
		( "output-first", bpo::value<std::ssize_t>( ), "specify the first output image, in order to retime the sequence. It's another way to create an offset of your sequence." )
		( "output-last" , bpo::value<std::ssize_t>( ), "specify the last output image, in order to retime the sequence" )
		("color"        , "display with colors")
		;

	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options( )
		( "input-dir", bpo::value< std::vector<std::string> >( ), "input directories" )
		("enable-color", bpo::value<std::string>(), "enable (or disable) color")
		;

	// define default options 
	bpo::positional_options_description pod;
	pod.add( "input-dir", -1 );

	bpo::options_description cmdline_options;
	cmdline_options.add( mainOptions ).add( hidden );

	bpo::positional_options_description pd;
	pd.add( "", -1 );

	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	try
	{
		bpo::store( bpo::command_line_parser( argc, argv ).options( cmdline_options ).positional( pod ).run( ), vm );

		// get environnement options and parse them
		if( const char* env_options = std::getenv( SAM_MV_OR_CP_OPTIONS ) )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify( vm );
	}
	catch( const bpo::error& e)
	{
		TUTTLE_COUT( SAM_TOOL ": command line error:  " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_COUT( SAM_TOOL ": unknown error in command line.");
		exit( -2 );
	}

	if ( vm.count("color") )
	{
		colorOutput = true;
	}
	if ( vm.count("enable-color") )
	{
		std::string str = vm["enable-color"].as<std::string>();

		if( str == "1" || boost::iequals(str, "y") || boost::iequals(str, "Y") || boost::iequals(str, "yes") || boost::iequals(str, "Yes") || boost::iequals(str, "true") || boost::iequals(str, "True") )
		{
			colorOutput = true;
		}
		else
		{
			colorOutput = false;
		}
	}

	if( colorOutput )
	{
		using namespace tuttle::common;
		sColorStd    = kColorStd;
		sColorBlue   = kColorFolder;
		sColorFolder = kColorFolder;
		sColorGreen  = kColorFile;
		sColorRed    = kColorError;
		sColorError  = kColorError;
	}

	if( vm.count( "help" ) )
	{
		TUTTLE_COUT( sColorBlue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << sColorStd << std::endl );
#ifndef SAM_MOVEFILES
		TUTTLE_COUT( sColorBlue  <<"NAME" << sColorStd );
		TUTTLE_COUT( sColorGreen << "\tsam-cp - copy sequence(s) in a directory" << sColorStd << std::endl );
		TUTTLE_COUT( sColorBlue  << "SYNOPSIS" << sColorStd );
		TUTTLE_COUT( sColorGreen << "\tsam-cp [options] sequence[s] [outputDirectory][outputSequence]" << sColorStd << std::endl );
#else
		TUTTLE_COUT( sColorBlue  << "NAME" << sColorStd );
		TUTTLE_COUT( sColorGreen << "\tsam-mv - move sequence(s) in a directory" << sColorStd << std::endl );
		TUTTLE_COUT( sColorBlue  << "SYNOPSIS" );
		TUTTLE_COUT( sColorGreen << "\tsam-mv [options] sequence[s] [outputDirectory][outputSequence]" << sColorStd << std::endl );
#endif
		TUTTLE_COUT( sColorBlue  << "DESCRIPTION" << sColorStd << std::endl );
#ifndef SAM_MOVEFILES
		TUTTLE_COUT( "Copy sequence of image files, and could remove trees (folder, files and sequences)." << std::endl );
#else
		TUTTLE_COUT( "Move sequence of image files, and could remove trees (folder, files and sequences)." << std::endl );
#endif
		TUTTLE_COUT( sColorBlue  << "OPTIONS" << sColorStd );
		TUTTLE_COUT( mainOptions );
		return 1;
	}

	if( vm.count( "expression" ) )
	{
		bal::split( filters, vm["expression"].as<std::string > ( ), bal::is_any_of( "," ) );
	}

	if( vm.count( "all" ) )
	{
		// add .* files
		descriptionMask |= ttl::eMaskOptionsDotFile;
	}

	// defines paths
	if( vm.count( "input-dir" ) )
	{
		paths = vm["input-dir"].as< std::vector<std::string> >( );
	}

	if( paths.size( ) < 2 )
	{
		TUTTLE_COUT( sColorError << "No sequence and/or directory are specified." << sColorStd );
		return 1;
	}

	if( vm.count( "offset" ) )
	{
		offset = vm["offset"].as<std::ssize_t>( );
		offsetMode = eOffsetModeValue;
	}

	if( vm.count( "input-first" ) )
	{
		hasInputFirst = true;
		inputFirst = vm["input-first"].as<std::ssize_t>( );
	}

	if( vm.count( "input-last" ) )
	{
		hasInputLast = true;
		inputLast = vm["input-last"].as<std::ssize_t>( );
	}

	if( vm.count( "output-first" ) )
	{
		outputFirst = vm["output-first"].as<std::ssize_t>( );
		if( offsetMode != eOffsetModeNotSet )
		{
			TUTTLE_CERR( sColorError << "You can't cumulate multiple options to modify the time." << sColorStd );
			return -1;
		}
		offsetMode = eOffsetModeFirstTime;
	}

	if( vm.count( "output-last" ) )
	{
		outputLast = vm["output-last"].as<std::ssize_t>( );
		if( offsetMode != eOffsetModeNotSet )
		{
			TUTTLE_CERR( sColorError << "You can't cumulate multiple options to modify the time." << sColorStd );
			return -1;
		}
		offsetMode = eOffsetModeLastTime;
	}

	if( vm.count( "verbose" ) )
	{
		verbose = true;
	}

	bfs::path dstPath = paths.back( );
	paths.pop_back( );
	std::string sequencePattern;

	if( ! bfs::is_directory( dstPath ) )
	{
		sequencePattern = dstPath.filename( ).string( );
		dstPath = dstPath.parent_path( );

		if( ! dstPath.empty() && ! bfs::is_directory( dstPath ) )
		{
			TUTTLE_CERR( sColorError << "Your destination is not in a valid directory: " << tuttle::quotes(dstPath.string()) << "." << sColorStd );
			return -1;
		}
	}
	else
	{
		if( paths.size( ) > 1 )
		{
			TUTTLE_CERR( sColorError << "To copy multiple sequences, your destination must be a directory: " << tuttle::quotes(dstPath.string()) << "." << sColorStd );
			return -1;
		}
		sequencePattern = "";
	}
	
	ttl::Sequence dstSeq( dstPath, descriptionMask );

	if( sequencePattern.size( ) > 0 )
	{
		dstIsSeq = dstSeq.initFromPattern( dstPath, sequencePattern, 0, 0, 1, descriptionMask, ttl::Sequence::ePatternAll );
		if( ! dstIsSeq ) // there is a pattern, but it's not valid.
		{
			TUTTLE_CERR( sColorError << "Your destination " << tuttle::quotes(sequencePattern) << " is not a valid pattern. Your destination can be a directory or a pattern." << sColorStd );
			return -1;
		}
	}

	try
	{
		BOOST_FOREACH( const bfs::path& srcPath, paths )
		{
			ttl::Sequence srcSeq( srcPath.branch_path( ), descriptionMask );
			const bool srcIsSeq = srcSeq.initFromDetection( srcPath.string( ), ttl::Sequence::ePatternDefault );
			if( ! srcIsSeq )
			{
				TUTTLE_CERR( sColorError << "Input is not a sequence: " << tuttle::quotes(srcPath.string()) << "."  << sColorStd );
				return -1;
			}
			if( srcSeq.getNbFiles( ) == 0 )
			{
				TUTTLE_CERR( sColorError << "No existing file for the input sequence: " << tuttle::quotes(srcPath.string()) << "." << sColorStd );
				return -1;
			}

			ttl::Sequence::Time first = hasInputFirst ? inputFirst : srcSeq.getFirstTime();
			ttl::Sequence::Time last = hasInputLast ? inputLast : srcSeq.getLastTime();
			switch( offsetMode )
			{
				case eOffsetModeNotSet:
				{
					offset = 0;
					break;
				}
				case eOffsetModeValue:
				{
					// set by "offset" command line option
					break;
				}
				case eOffsetModeFirstTime:
				{
					// set by "input-first" command line option
					offset = outputFirst - first;
					break;
				}
				case eOffsetModeLastTime:
				{
					// set by "input-last" command line option
					offset = outputLast - last;
					break;
				}
			}
			if( dstIsSeq )
			{
				if( verbose )
				{
					TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern( ) << " -> " << dstSeq.getAbsoluteStandardPattern( ) << " (" << srcSeq.getNbFiles( ) << ") " );
				}
				copy_sequence( srcSeq, first, last, dstSeq, offset );
			}
			else
			{
				if( verbose )
				{
					TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern( ) << " -> " << dstPath / srcSeq.getStandardPattern( ) << " (" << srcSeq.getNbFiles( ) << ")" );
				}
				copy_sequence( srcSeq, first, last, dstPath, offset );
			}
		}
	}
	catch( bfs::filesystem_error &ex )
	{
		TUTTLE_COUT( ex.what( ) );
		return -2;
	}
	catch( ... )
	{
		TUTTLE_CERR( boost::current_exception_diagnostic_information( ) );
		return -3;
	}

	return 0;
}

