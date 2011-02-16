#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>

#include <algorithm>
#include <iostream>

int main( int argc, char** argv )
{
	using namespace tuttle::common;
	namespace fs = boost::filesystem;

	try
	{
		std::vector<std::string> args;
		args.reserve( argc - 1 );

		if( argc <= 1 ) // no argument
		{
			args.push_back( "." );
		}
		else
		{
			for( int i = 1; i < argc; ++i )
				args.push_back( argv[i] );
		}

		BOOST_FOREACH( boost::filesystem::path path, args )
		{
			//TUTTLE_COUT( "param:" << path );

			if( fs::exists( path ) )
			{
				if( fs::is_directory( path ) )
				{
					std::vector<Sequence> sequences = tuttle::common::sequencesInDir( path );
					std::sort( sequences.begin(), sequences.end() );
					BOOST_FOREACH( const std::vector<Sequence>::value_type & s, sequences )
					{
						//						if( s.getNbFiles() != 1 )
						TUTTLE_COUT( s );
					}
				}
				else
				{
					Sequence s;
					s.initFromDetection( path, Sequence::ePatternFrame );
					if( s.getNbFiles() )
					{
						TUTTLE_COUT( s );
					}
					//					cout << "* File: " << path << endl;
				}
			}
			else
			{
				try
				{
					Sequence s;
					s.initFromDetection( path );
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
	catch(... )
	{
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}
	return 0;
}

