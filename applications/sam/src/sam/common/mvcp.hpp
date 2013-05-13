#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/exceptions.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <Sequence.hpp>

#include <algorithm>
#include <iostream>

#ifndef SAM_MOVEFILES
#define SAM_MV_OR_CP_OPTIONS    "SAM_CP_OPTIONS"
#define SAM_TOOL                "sam-cp"
#else
#define SAM_MV_OR_CP_OPTIONS    "SAM_MV_OPTIONS"
#define SAM_TOOL                "sam-mv"
#endif

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bal = boost::algorithm;
namespace sp  = sequenceParser;

void copy_sequence( const sp::Sequence& s, const sp::Time firstImage, const sp::Time lastImage, const sp::Sequence& d, int offset = 0 )
{
	sp::Time begin;
	sp::Time end;
	sp::Time step;
	if (offset > 0) {
		begin = lastImage;
		end = firstImage;
		step = -s.getStep();
	} else {
		begin = firstImage;
		end = lastImage;
		step = s.getStep();
	}
	
	for( sp::Time t = begin;
		 (offset > 0) ? (t >= end) :
		 (t <= end); t += step )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt(t);
		//TUTTLE_TLOG_VAR( TUTTLE_TRACE, sFile );
		if( bfs::exists( sFile) )
		{
			bfs::path dFile = d.getAbsoluteFilenameAt(t + offset);
			//TUTTLE_TLOG( TUTTLE_TRACE, "do " << sFile << " -> " << dFile );
#ifndef SAM_MOVEFILES // copy file(s)
			if( bfs::exists(dFile) )
			{
				TUTTLE_LOG_ERROR( "Could not copy: " << dFile.string( ) );
			}
			else
			{
				try
				{
					//TUTTLE_LOG_TRACE( "copy " << sFile << " -> " << dFile );
					bfs::copy_file(sFile, dFile);
				}
				catch (const bpo::error& e)
				{
					TUTTLE_LOG_ERROR( "error : " << e.what() );
				}
				catch (...)
				{
					TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information( ) );
				}
			}
#else // move file(s)
			if( bfs::exists( dFile ) )
			{
				TUTTLE_LOG_ERROR( "Could not move: " << dFile.string( ) );
			}
			else
			{
				try
				{
					//TUTTLE_LOG_TRACE( "move " << sFile << " -> " << dFile );
					bfs::rename( sFile, dFile );
				}
				catch( const bpo::error& e )
				{
					TUTTLE_LOG_ERROR( "error : " << e.what() );
				}
				catch( ... )
				{
					TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information( ) );
				}
			}
#endif
		}
	}
}

void copy_sequence( const sp::Sequence& s, const sp::Sequence& d, const sp::Time offset = 0 )
{
	copy_sequence(s, s.getFirstTime(), s.getLastTime(), d, offset);
}

void copy_sequence( const sp::Sequence& s, const sp::Time firstImage, const sp::Time lastImage, const bfs::path& d, const sp::Time offset = 0 )
{
	sp::Sequence dSeq( s ); // create dst from src
	dSeq.setDirectory( d ); // modify path
	copy_sequence( s, firstImage, lastImage, dSeq, offset );
}

void copy_sequence( const sp::Sequence& s, const bfs::path& d, const sp::Time offset = 0 )
{
	copy_sequence( s, s.getFirstTime(), s.getLastTime(), d, offset );
}

int sammvcp(int argc, char** argv)
{

	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );
	
	sp::EMaskOptions descriptionMask = sp::eMaskOptionsNone; // by default show nothing
	std::vector<std::string> paths;
	std::vector<std::string> filters;

	std::ssize_t offset = 0;
	std::ssize_t inputFirst = 0;
	std::ssize_t inputLast = 0;
	std::ssize_t outputFirst = 0;
	std::ssize_t outputLast = 0;
	
	bool dstIsSeq = false;
	bool hasInputFirst = false;
	bool hasInputLast = false;
	
	
	typedef enum {
		eOffsetModeNotSet,
		eOffsetModeValue,
		eOffsetModeFirstTime,
		eOffsetModeLastTime,
	} EOffsetMode;
	EOffsetMode offsetMode = eOffsetModeNotSet;

	formatter->init_logging();
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
			( kHelpOptionString,  kHelpOptionMessage )
			( kOffsetOptionString,      bpo::value<std::ssize_t>(), kOffsetOptionMessage ) 
			//		( "force,f"     , bpo::value<bool>( )        , "if a destination file exists, replace it" )
			( kVerboseOptionString,     bpo::value<int>()->default_value( 2 ), kVerboseOptionMessage )
			( kQuietOptionString, kQuietOptionMessage )
			( kInputFirstOptionString,  bpo::value<std::ssize_t>(), kInputFirstOptionMessage )
			( kInputLastOptionString,   bpo::value<std::ssize_t>(), kInputLastOptionMessage )
			( kOutputFirstOptionString, bpo::value<std::ssize_t>(), kOutputFirstOptionMessage )
			( kOutputLastOptionString,  bpo::value<std::ssize_t>(), kOutputLastOptionMessage )
			( kColorOptionString, kColorOptionMessage )
			( kBriefOptionString, kBriefOptionMessage );
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
			( kInputOptionString, bpo::value<std::vector<std::string> >(), kInputOptionMessage )
			( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );

	// define default options
	bpo::positional_options_description pod;
	pod.add( kInputOptionLongName, -1 );
	
	bpo::options_description cmdline_options;
	cmdline_options.add( mainOptions ).add( hidden );
	
	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	try
	{
		bpo::store( bpo::command_line_parser( argc, argv ).options( cmdline_options ).positional( pod ).run(), vm );
		
		// get environnement options and parse them
		if( const char* env_options = std::getenv( SAM_MV_OR_CP_OPTIONS ) )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_options, " " );
			bpo::store(bpo::command_line_parser( vecOptions ).options( cmdline_options ).positional( pod ).run(), vm );
		}
		if( const char* env_options = std::getenv( "SAM_OPTIONS" ) )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_options, " " );
			bpo::store(bpo::command_line_parser( vecOptions ).options(cmdline_options ).positional( pod ).run(), vm );
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e )
	{
		TUTTLE_LOG_ERROR( SAM_TOOL ": command line error:  " << e.what() );
		exit(-2);
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( SAM_TOOL ": unknown error in command line." );
		exit(-2);
	}

	if( vm.count( kColorOptionLongName ) )
	{
		color->enable();
	}
	if( vm.count( kEnableColorOptionLongName ) )
	{
		const std::string str = vm[kEnableColorOptionLongName].as<std::string>();
		if( string_to_boolean(str) )
		{
			color->enable();
		}
		else
		{
			color->disable();
		}
	}
	
	// defines paths
	if( vm.count( kInputOptionLongName ) )
	{
		paths = vm[ kInputOptionLongName ].as<std::vector<std::string> >();
	}


	if( vm.count( kBriefOptionLongName ) )
	{
#ifndef SAM_MOVEFILES
		TUTTLE_LOG_INFO( color->_green << "copy sequence(s) in a directory" << color->_std);
#else
		TUTTLE_LOG_INFO( color->_green << "move sequence(s) in a directory" << color->_std );
#endif
		return 0;
	}

	bool isPathSizeTooSmall = ( paths.size() < 2 );
	if( vm.count( kHelpOptionLongName ) || isPathSizeTooSmall )
	{
		if( isPathSizeTooSmall && !vm.count( kHelpOptionLongName ) )
			TUTTLE_LOG_ERROR( "Two sequences and/or directories must be specified." );
		
		TUTTLE_LOG_INFO( color->_blue << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << color->_std );
		TUTTLE_LOG_INFO( "" );
#ifndef SAM_MOVEFILES
		TUTTLE_LOG_INFO( color->_blue <<"NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-cp - copy sequence(s) in a directory" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-cp [options] sequence[s] [outputDirectory][outputSequence]" << color->_std );
#else
		TUTTLE_LOG_INFO( color->_blue << "NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-mv - move sequence(s) in a directory" << color->_std);
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-mv [options] sequence[s] [outputDirectory][outputSequence]" << color->_std );
#endif
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue << "DESCRIPTION" << color->_std );
		TUTTLE_LOG_INFO( "" );
#ifndef SAM_MOVEFILES
		TUTTLE_LOG_INFO( "Copy sequence of image files, and could remove trees (folder, files and sequences)." );
#else
		TUTTLE_LOG_INFO( "Move sequence of image files, and could remove trees (folder, files and sequences)." );
#endif
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue << "OPTIONS" << color->_std );
		TUTTLE_LOG_INFO( mainOptions );
		/////Examples
		
		TUTTLE_LOG_INFO( color->_blue << "EXAMPLES" << color->_std << std::left );
		SAM_EXAMPLE_TITLE_COUT( "Sequence possible definitions: " );
		SAM_EXAMPLE_LINE_COUT ( "Auto-detect padding : ", "seq.@.jpg" );
		SAM_EXAMPLE_LINE_COUT ( "Padding of 8 (usual style): ", "seq.########.jpg" );
		SAM_EXAMPLE_LINE_COUT ( "Padding of 8 (printf style): ", "seq.%08d.jpg" );
#ifndef SAM_MOVEFILES
		SAM_EXAMPLE_TITLE_COUT( "Copy a sequence: " );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-cp /path/to/sequence/seq.@.jpg  /path/to/sequence_copy/" );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-cp /path/to/sequence/seq.@.jpg  /path/to/sequences_copy/seq.@.jpg" );
		SAM_EXAMPLE_TITLE_COUT( "Copy and rename a sequence: " );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-cp /path/to/sequence/seq.@.jpg  /path/to/sequence_copy/seq_copy.@.jpg " );
		SAM_EXAMPLE_TITLE_COUT( "Copy a part of sequence: " );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-cp /path/to/sequence/seq.@.jpg  /path/to/sequence_copy/ --input-first 677837 --input-last 677838" );
		SAM_EXAMPLE_TITLE_COUT( "Renumber a sequence: " );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-cp /path/to/sequence/seq.@.jpg  /path/to/sequence_copy/ --output-first 0" );
#else
		SAM_EXAMPLE_TITLE_COUT( "Move a sequence: ");
		SAM_EXAMPLE_LINE_COUT ( "", "sam-mv /path/to/sequence/seq.@.jpg  /path/to/sequence_move/" );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-mv /path/to/sequence/seq.@.jpg  /path/to/sequences_move/seq.@.jpg" );
		SAM_EXAMPLE_TITLE_COUT( "Move and rename a sequence:" );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-mv /path/to/sequence/seq.@.jpg  /path/to/sequence_move/seq_move.@.jpg" );
		SAM_EXAMPLE_TITLE_COUT( "Move a part of sequence:" );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-mv /path/to/sequence/seq.@.jpg  /path/to/sequence_move/ --input-first 677837 --input-last 677838" );
		SAM_EXAMPLE_TITLE_COUT( "Renumber a sequence:" );
		SAM_EXAMPLE_LINE_COUT ( "", "sam-mv /path/to/sequence/seq.@.jpg  /path/to/sequence_move/ --output-first 0" );
		
#endif
		return 1;
	}

	if( vm.count(kExpressionOptionLongName) )
	{
		bal::split(filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}
	
	if( vm.count(kAllOptionLongName) )
	{
		// add .* files
		descriptionMask |= sp::eMaskOptionsDotFile;
	}
	
	if( vm.count(kOffsetOptionLongName) )
	{
		offset = vm[kOffsetOptionLongName].as<std::ssize_t>();
		offsetMode = eOffsetModeValue;
	}
	
	if( vm.count(kInputFirstOptionLongName) )
	{
		hasInputFirst = true;
		inputFirst = vm[kInputFirstOptionLongName].as<std::ssize_t>();
	}
	
	if( vm.count(kInputLastOptionLongName) )
	{
		hasInputLast = true;
		inputLast = vm[kInputLastOptionLongName].as<std::ssize_t>();
	}
	
	if( vm.count(kOutputFirstOptionLongName) )
	{
		outputFirst = vm[kOutputFirstOptionLongName].as<std::ssize_t>();
		if( offsetMode != eOffsetModeNotSet )
		{
			TUTTLE_LOG_ERROR( "You can't cumulate multiple options to modify the time." );
			return -1;
		}
		offsetMode = eOffsetModeFirstTime;
	}

	if( vm.count(kOutputLastOptionLongName) )
	{
		outputLast = vm[kOutputLastOptionLongName].as<std::ssize_t>();
		if (offsetMode != eOffsetModeNotSet)
		{
			TUTTLE_LOG_ERROR( "You can't cumulate multiple options to modify the time." );
			return -1;
		}
		offsetMode = eOffsetModeLastTime;
	}
	
	switch( vm[ kVerboseOptionLongName ].as< int >() )
	{
		case 0 :  formatter->setLogLevel( boost::log::trivial::trace   ); break;
		case 1 :  formatter->setLogLevel( boost::log::trivial::debug   ); break;
		case 2 :  formatter->setLogLevel( boost::log::trivial::info    ); break;
		case 3 :  formatter->setLogLevel( boost::log::trivial::warning ); break;
		case 4 :  formatter->setLogLevel( boost::log::trivial::error   ); break;
		case 5 :  formatter->setLogLevel( boost::log::trivial::fatal   ); break;
		default : formatter->setLogLevel( boost::log::trivial::warning ); break;
	}
	if( vm.count(kQuietOptionLongName) )
	{
		formatter->setLogLevel( boost::log::trivial::fatal );
	}

	bfs::path dstPath = paths.back();
	paths.pop_back();
	std::string sequencePattern;
	
	if( !bfs::is_directory( dstPath ) )
	{
		sequencePattern = dstPath.filename().string();
		dstPath = dstPath.parent_path();
		
		if( ! dstPath.empty() && !bfs::is_directory( dstPath ) )
		{
			TUTTLE_LOG_ERROR( "Your destination is not in a valid directory: " << tuttle::quotes( dstPath.string() ) << "." );
			return -1;
		}
	}
	else
	{
		if( paths.size() > 1 )
		{
			TUTTLE_LOG_ERROR( "To copy multiple sequences, your destination must be a directory: " << tuttle::quotes( dstPath.string() ) << "." );
			return -1;
		}
		sequencePattern = "";
	}

	sp::Sequence dstSeq( dstPath, descriptionMask );
	
	if( sequencePattern.size() > 0 )
	{
		dstIsSeq = dstSeq.initFromPattern( dstPath, sequencePattern, 0, 0, 1, descriptionMask, sp::Sequence::ePatternAll );
		if( !dstIsSeq ) // there is a pattern, but it's not valid.
		{
			TUTTLE_LOG_ERROR("Your destination " << tuttle::quotes(sequencePattern) << " is not a valid pattern. Your destination can be a directory or a pattern." );
			return -1;
		}
	}
	
	try
	{
		BOOST_FOREACH( const bfs::path& srcPath, paths )
		{
			sp::Sequence srcSeq( srcPath.branch_path(), descriptionMask );
			const bool srcIsSeq = srcSeq.initFromDetection( srcPath.string(), sp::Sequence::ePatternDefault );
			if( ! srcIsSeq )
			{
				TUTTLE_LOG_ERROR( color->_error << "Input is not a sequence: " << tuttle::quotes( srcPath.string() ) << "." << color->_std );
				return -1;
			}
			if( srcSeq.getNbFiles() == 0 )
			{
				TUTTLE_LOG_ERROR( color->_error << "No existing file for the input sequence: " << tuttle::quotes( srcPath.string() ) << "." << color->_std );
				return -1;
			}
			
			sp::Time first = hasInputFirst ? inputFirst : srcSeq.getFirstTime();
			sp::Time last = hasInputLast ? inputLast : srcSeq.getLastTime();
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
				TUTTLE_LOG_TRACE( srcSeq.getAbsoluteStandardPattern( ) << " -> " << dstSeq.getAbsoluteStandardPattern( ) << " (" << srcSeq.getNbFiles( ) << ") " );
				copy_sequence( srcSeq, first, last, dstSeq, offset );
			}
			else
			{
				TUTTLE_LOG_TRACE( srcSeq.getAbsoluteStandardPattern( ) << " -> " << dstPath / srcSeq.getStandardPattern( ) << " (" << srcSeq.getNbFiles( ) << ")" );
				copy_sequence( srcSeq, first, last, dstPath, offset );
			}
		}
	}
	catch( bfs::filesystem_error &ex )
	{
		TUTTLE_LOG_ERROR( ex.what( ) );
		return -2;
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information( ) );
		return -3;
	}
	
	return 0;
}

