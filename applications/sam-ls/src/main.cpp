
#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	using namespace std;
	using namespace tuttle::common;
	namespace fs = boost::filesystem;

	try
	{
		if( argc < 2 )
		{
			cout << "no path.\n" << endl;
			return 1;
		}

		boost::filesystem::path path( argv[1] );
		if( fs::is_directory( path ) )
		{
			COUT( "-- is directory --" );
			std::vector<Sequence> sequences = sequencesInDir( path );
			BOOST_FOREACH( std::vector<Sequence>::value_type v, sequences )
			{
				COUT_X( 80, "_" );
				cout << "Sequence:" << endl;
				cout << v << endl;
			}
		}
		else
		{
			COUT( "-- is pattern --" );
			cout << "path:" << path << endl;
			Sequence clip( path );
			COUT_VAR( clip.getDirectory() );
			COUT_VAR( clip.getFirstFilename() );
			COUT_VAR( clip.getLastFilename() );
			COUT_VAR( clip.getStep() );
			COUT_VAR( clip.getFirstTime() );
			COUT_VAR( clip.getLastTime() );
			COUT_VAR( clip.getPadding() );
			COUT_VAR( clip.isStrictPadding() );
			COUT_VAR( clip.hasMissingFrames() );
			COUT_VAR( clip.getIdentification() );
			COUT_VAR( clip.getPrefix() );
			COUT_VAR( clip.getSuffix() );
		}


	}
	catch(...)
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
	}
	return 0;
}

