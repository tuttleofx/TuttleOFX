// for TUTTLE_COUT and TUTTLE_CERR
#include <tuttle/common/exceptions.hpp>


#include <boost/exception/diagnostic_information.hpp>
#include <iostream>

int main( int argc, char** argv)
{
	try
	{
		if( argc == 1 )
		{
			// list of plugins presents
			TUTTLE_COUT( "Listing of OpenFX plugins:\n" );
			return 0;
		}
		else
		{
			// list specifications of plugin(s)
			std::vector<std::string> args;
			args.reserve( argc - 1 );
			for( int i = 1; i < argc; ++i )
				args.push_back( argv[i] );
			
			TUTTLE_COUT( "Specifications of OpenFX plugin(s)\n" );
			for( unsigned int i = 0; i < args.size(); ++i )
			{
			  TUTTLE_COUT( args.at(i) << " :\n" );
			}
			// alls plugins names are in args
		}
		
	}
	catch(... )
	{
	  TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}
	return 0;
}
