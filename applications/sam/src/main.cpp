
#include <tuttle/host/core/Graph.hpp>

int main( int argc, char** argv )
{
	try
	{
		using namespace tuttle::host;
		//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
		//core::Core::instance().getPluginCache().scanPluginFiles();
		core::Core::instance().preload();
		core::Core::instance().getImageEffectPluginCache().dumpToStdOut();

		TCOUT( "__________________________________________________1" );

		core::Graph g;
		core::Graph::Node& read1   = g.createNode( "fr.hd3d.tuttle.pngreader" );
		core::Graph::Node& denoiser1 = g.createNode( "fr.duranduboi.duplo.nlmdenoiser" );
		core::Graph::Node& write1  = g.createNode( "fr.hd3d.tuttle.pngwriter" );

		TCOUT( "__________________________________________________2" );
		// Setup parameters
		read1.getParam( "Input filename" ).set( "input.png" );
	//	crop1.getParam( "Down" ).set( 400 );
		write1.getParam( "Output filename" ).set( "output.png" );
		OfxPointD renderScale = { 1.0, 1.0 };
		read1.paramInstanceChangedAction( "Input filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
	//	crop1.paramInstanceChangedAction( "Down", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
		write1.paramInstanceChangedAction( "Output filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );

		TCOUT( "__________________________________________________3" );
		g.connect( read1, denoiser1 );
		g.connect( denoiser1, write1 );

		TCOUT( "__________________________________________________4" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		g.compute( outputs, 0, 1 );

		TCOUT( "__________________________________________________5" );

	}
	catch( tuttle::host::core::exception::LogicError& e )
	{
		std::cout << "Tuttle Exception : main de tuttle..." << std::endl;
		std::cout << e.what() << std::endl;
	}
	catch( std::exception& e )
	{
		std::cout << "Std Exception : main de tuttle..." << std::endl;
		std::cout << e.what() << std::endl;
	}
	catch(... )
	{
		std::cout << "Exception..." << std::endl;
	}

	//    COUT_INFOS;
	return 0;
}

