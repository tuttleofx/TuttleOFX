#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>

#include <iostream>

namespace bfs = boost::filesystem;
using namespace tuttle::common;

void copy_sequence( const Sequence s, const Sequence d )
{
	for( Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt(t);
		if( bfs::exists( sFile ) )
		{
			bfs::path dFile = d.getAbsoluteFilenameAt(t);
			bfs::copy_file( sFile, dFile );
		}
	}
}

void copy_sequence( const Sequence s, const bfs::path d )
{
	for( Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
	{
		bfs::path sFile = s.getFilenameAt(t);
		if( bfs::exists( sFile ) )
		{
			bfs::copy_file( s.getDirectory() / sFile, d / sFile );
		}
	}
}

int main( int argc, char** argv )
{

	try
	{
		if( argc == 1 )
		{
			CERR( "No argument." );
			return -1;
		}
		else if( argc == 2 )
		{
			CERR( "Where do you want to copy \"" << argv[1] << "\"?" );
			return -1;
		}
		std::vector<std::string> allSrc;
		allSrc.reserve( argc - 1 );
		for( int i = 1; i < argc - 1; ++i )
			allSrc.push_back( argv[i] );
		bfs::path dstPath( argv[argc-1] );
		bool dstIsDir = bfs::is_directory( dstPath );
		Sequence dstSeq;
		bool dstIsSeq = dstSeq.init( dstPath, 0, 0, 1, Sequence::ePatternAll );

		if( !dstIsDir && !dstIsSeq )
		{
			CERR( "Your destination must be a sequence or a directory." );
			return -1;
		}
		if( allSrc.size() > 1 && ! dstIsDir )
		{
			CERR( "To copy multiple sequences, your destination must be a directory." );
			return -1;
		}
		BOOST_FOREACH( bfs::path srcPath, allSrc )
		{
			Sequence srcSeq;
			bool srcIsSeq = srcSeq.initFromDetection( srcPath, Sequence::ePatternDefault );
			if( ! srcIsSeq )
			{
				CERR( "Input is not a sequence. (" << srcPath << ")" );
				return -1;
			}
			if( srcSeq.getNbFiles() == 0 )
			{
				CERR( "No existing file for the input sequence. (" << srcPath << ")" );
				return -1;
			}
			if( dstIsSeq )
			{
				COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstSeq.getAbsoluteStandardPattern() << " (" << srcSeq.getNbFiles() << ")" );
				copy_sequence( srcSeq, dstSeq );
			}
			else
			{
				COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstPath / srcSeq.getStandardPattern() << " (" << srcSeq.getNbFiles() << ")"  );
				copy_sequence( srcSeq, dstPath );
			}
		}
	}
	catch( const bfs::filesystem_error& e )
	{
		std::cerr << e.what() << std::endl;
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		return -1;
	}
	return 0;
}

