#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Detector.hpp>
#include <Sequence.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace sp  = sequenceParser;

bool         selectRange    = false;
std::ssize_t firstImage     = 0;
std::ssize_t lastImage      = 0;

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	copy( v.begin(), v.end(), std::ostream_iterator<T>( std::cout, " " ) );
	return os;
}

void removeSequence( const sp::Sequence& s )
{
	std::ssize_t first;
	std::ssize_t last;
	if( selectRange )
	{
		first = std::max( s.getFirstTime(), firstImage );
		last  = std::min( s.getLastTime(), lastImage );
	}
	else
	{
		first = s.getFirstTime();
		last  = s.getLastTime();
	}
//	TUTTLE_TCOUT( "remove sequence." );
//	TUTTLE_TCOUT("remove from " << first << " to " << last);

	for( sp::Time t = first; t <= last; t += s.getStep() )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt(t);
		if( !bfs::exists( sFile ) )
		{
			TUTTLE_LOG_ERROR( "Could not remove (file not exist): " << sFile.string() );
		}
		else
		{
			TUTTLE_LOG_TRACE( "remove: " << tuttle::common::Color::get()->_folder << sFile.string() << tuttle::common::Color::get()->_std );
			try
			{
				bfs::remove( sFile );
			}
			catch( const boost::filesystem::filesystem_error& e )
			{
//			   if( e.code() == boost::system::errc::permission_denied )
//				   "permission denied"
				TUTTLE_LOG_ERROR( "sam-rm: Error:\t\n" << e.what() );
				/// @todo cin
//				TUTTLE_LOG_INFO( "sam-rm: Continue ? (Yes/No/Yes for All/No for All)" );
			}
		}
	}
}

void removeFileObject( boost::ptr_vector<sp::FileObject> &listing, std::vector<boost::filesystem::path> &notRemoved )
{
//	TUTTLE_TCOUT( "removeFileObject." );
	BOOST_FOREACH( const sp::FileObject& s, listing )
	{
		if( !(s.getMaskType () == sp::eMaskTypeDirectory))
		{
			TUTTLE_LOG_TRACE( "remove: " << s );
			if( s.getMaskType () == sp::eMaskTypeSequence )
				removeSequence( static_cast<const sp::Sequence&>( s ) );
			else
			{
				std::vector<bfs::path> paths = s.getFiles();
				for(unsigned int i=0; i<paths.size(); i++)
					bfs::remove( paths.at(i) );
			}
		}
		else // is a directory
		{
			std::vector<boost::filesystem::path> paths = s.getFiles();
			for(unsigned int i=0; i<paths.size(); i++)
			{
				if( bfs::is_empty( paths.at(i) ) )
				{
					TUTTLE_LOG_TRACE( "remove: " << s );
					bfs::remove( paths.at(i) );
				}
				else
				{
					notRemoved.push_back( paths.at(i) );
				}
			}
		}
	}
}

void removeFiles( std::vector<boost::filesystem::path> &listing )
{
	std::sort(listing.begin(), listing.end());
	std::reverse(listing.begin(), listing.end());
	BOOST_FOREACH( const boost::filesystem::path& paths, listing )
	{
		if( bfs::is_empty( paths ) )
		{
			TUTTLE_LOG_TRACE( "remove: " << tuttle::common::Color::get()->_folder << paths << tuttle::common::Color::get()->_std );
			bfs::remove( paths );
		}
		else
		{
			TUTTLE_LOG_ERROR( "could not remove " << paths );
		}
	}
}


int main( int argc, char** argv )
{
	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );

	sp::EMaskType                researchMask      = sp::eMaskTypeSequence;	// by default show sequences
	sp::EMaskOptions             descriptionMask   = sp::eMaskOptionsColor;	// by default show nothing
	bool                         recursiveListing  = false;
	std::vector<std::string>     paths;
	std::vector<std::string>     filters;

	formatter->init_logging();
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
			( kAllOptionString,         kAllOptionMessage )
			( kDirectoriesOptionString, kDirectoriesOptionMessage )
			( kExpressionOptionString,  bpo::value<std::string>(), kExpressionOptionMessage )
			( kFilesOptionString,       kFilesOptionMessage )
			( kHelpOptionString,        kHelpOptionMessage )
			( kIgnoreOptionString,      kIgnoreOptionMessage )
			( kPathOptionString,        kPathOptionMessage )
			( kRecursiveOptionString,   kRecursiveOptionMessage )
			( kVerboseOptionString,     bpo::value<int>()->default_value( 2 ), kVerboseOptionMessage )
			( kQuietOptionString,       kQuietOptionMessage )
			( kColorOptionString,       kColorOptionMessage )
			( kFirstImageOptionString,  bpo::value<std::ssize_t>(), kFirstImageOptionMessage )
			( kLastImageOptionString,   bpo::value<std::ssize_t>(), kLastImageOptionMessage )
			( kFullRMPathOptionString,  kFullRMPathOptionMessage )
			( kBriefOptionString,       kBriefOptionMessage );
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
			( kInputDirOptionString, bpo::value< std::vector<std::string> >(), kInputDirOptionMessage )
			( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add(kInputDirOptionString, -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add( mainOptions ).add( hidden );

	bpo::positional_options_description pd;
	pd.add( "", -1 );
	
	//parse the command line, and put the result in vm
	bpo::variables_map vm;

	bpo::notify( vm );

	try
	{
		bpo::store( bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm );

		// get environment options and parse them
		if( const char* env_rm_options = std::getenv("SAM_RM_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_rm_options, " " );
			bpo::store( bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm );
		}
		if( const char* env_rm_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_rm_options, " " );
			bpo::store( bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm );
		}
	}
	catch( const bpo::error& e)
	{
		TUTTLE_LOG_ERROR( "sam-rm: command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR( "sam-rm: unknown error in command line." );
		exit( -2 );
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

	if( vm.count( kHelpOptionLongName ) )
	{
		TUTTLE_LOG_INFO( color->_blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-rm - remove file sequences" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-rm [options] [sequence_pattern]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "DESCRIPTION" << color->_std << std::endl );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( "Remove sequence of files, and could remove trees (folder, files and sequences)." );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "OPTIONS" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( mainOptions );

		TUTTLE_LOG_INFO( color->_blue << "EXAMPLES" << color->_std );
		SAM_EXAMPLE_TITLE_COUT( "Sequence possible definitions: " );
		SAM_EXAMPLE_LINE_COUT ( "Auto-detect padding : ", "seq.@.jpg" );
		SAM_EXAMPLE_LINE_COUT ( "Padding of 8 (usual style): ", "seq.########.jpg" );
		SAM_EXAMPLE_LINE_COUT ( "Padding of 8 (printf style): ", "seq.%08d.jpg" );
		SAM_EXAMPLE_TITLE_COUT( "Delete: " );
		SAM_EXAMPLE_LINE_COUT ( "A sequence:", "sam-rm /path/to/sequence/seq.@.jpg" );
		SAM_EXAMPLE_LINE_COUT ( "Sequences in a directory:", "sam-rm /path/to/sequence/" );
		TUTTLE_LOG_INFO( "" );

		return 0;
	}

	if( vm.count( kBriefOptionLongName) )
	{
		TUTTLE_LOG_INFO( color->_green << "remove file sequences" << color->_std );
		return 0;
	}

	if(vm.count( kExpressionOptionLongName))
	{
		bal::split( filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}

	if( vm.count( kDirectoriesOptionLongName ) )
	{
		researchMask |= sp::eMaskTypeDirectory;
	}
	
	if( vm.count( kFilesOptionLongName ) )
	{
		researchMask |= sp::eMaskTypeFile;
	}
	
	if( vm.count( kIgnoreOptionLongName ) )
	{
		researchMask &= ~sp::eMaskTypeSequence;
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

	if( vm.count( kFirstImageOptionLongName ) )
	{
		selectRange = true;
		firstImage  = vm[kFirstImageOptionLongName].as< std::ssize_t >();
	}

	if( vm.count( kLastImageOptionLongName ) )
	{
		selectRange = true;
		lastImage  = vm[kLastImageOptionLongName].as< std::ssize_t >();
	}

	if( vm.count( kFullRMPathOptionLongName ) )
	{
		researchMask |= sp::eMaskTypeDirectory;
		researchMask |= sp::eMaskTypeFile;
		researchMask |= sp::eMaskTypeSequence;
	}
	
	if( vm.count( kAllOptionLongName ) )
	{
		// add .* files
		descriptionMask |= sp::eMaskOptionsDotFile;
	}
	
	if( vm.count( kPathOptionLongName ) )
	{
		descriptionMask |= sp::eMaskOptionsPath;
	}
	
	// defines paths, but if no directory specify in command line, we add the current path
	if( vm.count( kInputDirOptionLongName ) )
	{
		paths = vm[kInputDirOptionLongName].as< std::vector<std::string> >();
	}
	else
	{
		TUTTLE_LOG_ERROR( "No sequence and/or directory are specified." );
		return 1;
	}

	if( vm.count( kRecursiveOptionLongName ) )
	{
		recursiveListing = true;
	}
	// 	for(unsigned int i=0; i<filters.size(); i++)
	// 	TUTTLE_COUT("filters = " << filters.at(i));
	// 	TUTTLE_COUT("research mask = " << researchMask);
	// 	TUTTLE_COUT("options  mask = " << descriptionMask);

	try
	{
		std::vector<boost::filesystem::path> pathsNoRemoved;
		sp::Detector detector;
		BOOST_FOREACH( bfs::path path, paths )
		{
//			TUTTLE_TCOUT( "path: "<< path );
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{
					//TUTTLE_TCOUT( "is a directory" );
					if(recursiveListing)
					{
						for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
						{
							if( bfs::is_directory( *dir ) )
							{
								//TUTTLE_TCOUT( *dir );
								boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( ( (bfs::path) *dir ).string(), filters, researchMask, descriptionMask );
								removeFileObject( listing, pathsNoRemoved );
							}
						}
					}
					boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( path.string(), filters, researchMask, descriptionMask );
					removeFileObject( listing, pathsNoRemoved );
				}
				else
				{
					//TUTTLE_TCOUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( path.branch_path().string(), filters, researchMask, descriptionMask );
					removeFileObject( listing, pathsNoRemoved );
				}
			}
			else
			{
//				TUTTLE_TCOUT( "not exist ...." );
				try
				{
					sp::Sequence s(path.branch_path(), descriptionMask );
					s.initFromDetection( path.string(), sp::Sequence::ePatternDefault );
					if( s.getNbFiles() )
					{
//						TUTTLE_TCOUT( s );
						removeSequence( s );
					}
				}
				catch(... )
				{
					TUTTLE_LOG_ERROR( "Unrecognized pattern \"" << path << "\"" );
				}
			}
		}
		// delete not empty folder the first time
		removeFiles( pathsNoRemoved );
	}
	catch( bfs::filesystem_error &ex )
	{
		TUTTLE_LOG_ERROR( ex.what() );
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information() );
	}
	return 0;
}

