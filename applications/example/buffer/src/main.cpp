#include <tuttle/host/Graph.hpp>

void sam_terminate( void )
{
	std::cerr << "Sorry, Sam has encountered a fatal error." << std::endl;
	std::cerr << "Please report this bug." << std::endl;
	exit( -1 );
}

void sam_unexpected( void )
{
	std::cerr << "Sorry, Sam has encountered an unexpected exception." << std::endl;
	std::cerr << "Please report this bug." << std::endl;
	BOOST_THROW_EXCEPTION( std::runtime_error( "Sorry, Sam has encountered an unexpected exception.\nPlease report this bug." ) );
}

int main( int argc, char** argv )
{
	std::set_terminate( &sam_terminate );
	std::set_unexpected( &sam_unexpected );
	try
	{
		using namespace tuttle::host;
		TUTTLE_COUT( "__________________________________________________0" );
		// Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
		// Core::instance().getPluginCache().scanPluginFiles();
		Core::instance().preload();

		TUTTLE_COUT( Core::instance().getImageEffectPluginCache() );

		TUTTLE_COUT( "__________________________________________________1" );

		Graph g;
		//		Graph::Node& read1   = g.createNode( "fr.tuttle.ffmpegreader" );
		Graph::Node& read1    = g.createNode( "fr.tuttle.pngreader" );
		Graph::Node& read2    = g.createNode( "fr.tuttle.dpxreader" );
		Graph::Node& read3    = g.createNode( "fr.tuttle.exrreader" );
		Graph::Node& bitdepth = g.createNode( "fr.tuttle.bitdepth" );
		Graph::Node& invert1  = g.createNode( "fr.tuttle.invert" );
		/*Graph::Node& invert2 = */ g.createNode( "fr.tuttle.invert" );
		/*Graph::Node& invert2 = */ g.createNode( "fr.tuttle.invert" );
		Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
		Graph::Node& blur1   = g.createNode( "fr.tuttle.blur" );
		Graph::Node& invert4 = g.createNode( "fr.tuttle.invert" );
		//	Graph::Node& crop1   = g.createNode( "fr.tuttle.crop" );
		Graph::Node& merge1    = g.createNode( "fr.tuttle.merge" );
		Graph::Node& bitdepth1 = g.createNode( "fr.tuttle.bitdepth" );

		
		TUTTLE_COUT( "__________________________________________________2" );
		// Setup parameters
		//		read1.getParam( "filename" ).set( "data/input1.avi" );
		read1.getParam( "filename" ).set( "data/input.png" );
		read2.getParam( "filename" ).set( "data/input.dpx" );
		read3.getParam( "filename" ).set( "data/input.exr" );
		bitdepth.getParam( "outputBitDepth" ).set( 3 );
		TUTTLE_COUT_VAR( bitdepth.getParam( "outputBitDepth" ).getString() );
		blur1.getParam( "size" ).set( 6.5, 15.0 );
		//		blur1.getParam( "size" ).setAtIndex( 65.43, 1 );
		//	crop1.getParam( "Down" ).set( 400 );

		TUTTLE_COUT( "__________________________________________________3" );
		g.connect( read1, bitdepth );
		g.connect( bitdepth, invert1 );
		g.connect( invert1, invert2 );
		g.connect( invert2, blur1 );
		g.connect( invert1, invert4 );

		g.connect( invert1, bitdepth1 );
		g.connect( bitdepth1, merge1.getAttribute( "SourceA" ) );
		g.connect( read3, merge1.getAttribute( "SourceB" ) );

		TUTTLE_COUT( "__________________________________________________4" );
		std::list<std::string> outputs;
		outputs.push_back( merge1.getName() );
		outputs.push_back( invert1.getName() );
		
		memory::MemoryCache values = g.compute( outputs, 0, 0 );
		TUTTLE_COUT( "__________________________________________________5" );
		TUTTLE_COUT_VAR( invert1.getName() );
		memory::CACHE_ELEMENT img = values.get( invert1.getName(), 0 );

		if( ! img.get() )
		{
			std::cout << "No buffer..." << std::endl;
			return 1;
		}
		TUTTLE_COUT( "__________________________________________________6" );
		ofx::imageEffect::EBitDepth bd = img->getBitDepth();
		ofx::imageEffect::EPixelComponent pc = img->getComponentsType();
		OfxRectI bounds = img->getBounds();

		TUTTLE_COUT_VAR( mapBitDepthEnumToString( bd ) );
		TUTTLE_COUT_VAR( mapPixelComponentEnumToString( pc ) );
		TUTTLE_COUT_VAR4( bounds.x1, bounds.y1, bounds.x2, bounds.y2 );
		TUTTLE_COUT_VAR( img->getClipName() );
		TUTTLE_COUT_VAR( img->getPixelData() );
	}
	catch( tuttle::exception::Common& e )
	{
		std::cout << "Tuttle Exception : main de sam." << std::endl;
		std::cerr << boost::diagnostic_information( e );
	}
	catch(... )
	{
		std::cerr << "Exception ... : main de sam." << std::endl;
		std::cerr << boost::current_exception_diagnostic_information();

	}

	return 0;
}

