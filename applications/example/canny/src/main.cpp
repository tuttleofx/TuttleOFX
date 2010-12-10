#include <tuttle/host/Graph.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/gil/image_view_factory.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "tuttle/host/InputBufferNode.hpp"

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
		TCOUT( "__________________________________________________0" );
		// Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
		// Core::instance().getPluginCache().scanPluginFiles();
		Core::instance().preload();

		TCOUT( Core::instance().getImageEffectPluginCache() );

		TCOUT( "__________________________________________________1" );

		boost::gil::rgba32f_image_t imgRead;
		boost::gil::png_read_and_convert_image( argv[1], //"data/input.png",
											    imgRead );
		boost::gil::rgba32f_view_t imgView( view( imgRead ) );

		TCOUT( "__________________________________________________2" );

		Graph g;
//		Graph::Node& inputBuffer1 = g.createNode( "fr.tuttle.inputbuffer" );
		InputBufferNode& inputBuffer1 = g.createInputBuffer();
//		Graph::Node& read        = g.createNode( "fr.tuttle.pngreader" );
		Graph::Node& bitdepth    = g.createNode( "fr.tuttle.bitdepth" );
		Graph::Node& blur        = g.createNode( "fr.tuttle.blur" );
		Graph::Node& sobel1      = g.createNode( "fr.tuttle.duranduboi.sobel" );
		Graph::Node& sobel2      = g.createNode( "fr.tuttle.duranduboi.sobel" );
//		Graph::Node& normalize   = g.createNode( "fr.tuttle.duranduboi.normalize" );
		Graph::Node& localmaxima = g.createNode( "fr.tuttle.duranduboi.localmaxima" );
		Graph::Node& floodfill   = g.createNode( "fr.tuttle.duranduboi.floodfill" );
		Graph::Node& thinning    = g.createNode( "fr.tuttle.duranduboi.thinning" );
		Graph::Node& write1      = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write2      = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write3      = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write4      = g.createNode( "fr.tuttle.pngwriter" );

		TCOUT( "__________________________________________________3" );

		OfxRectD ibRod = { 0, 0, imgView.width(), imgView.height() };
		inputBuffer1.setClipRod( ibRod );
		inputBuffer1.setClipBitDepth( InputBufferNode::eBitDepthFloat );
		inputBuffer1.setClipComponent( InputBufferNode::ePixelComponentRGBA );
		inputBuffer1.setClipRawBuffer( /*static_cast<char*>*/(char*)(boost::gil::interleaved_view_get_raw_data( imgView )) );

		// Setup parameters
		//		read1.getParam( "filename" ).set( "data/input1.avi" );
//		read1.getParam( "filename" ).set( "data/input.png" );
		bitdepth.getParam( "outputBitDepth" ).set( 3 );

		blur.getParam( "border" ).set( 3 );
		blur.getParam( "size" ).set( 1.0, 1.0 );
		blur.getParam( "normalizedKernel" ).set( false );
		blur.getParam( "kernelEpsilon" ).set( 0.01 );
		sobel1.getParam( "border" ).set( 3 );
		sobel1.getParam( "size" ).set( 1.5, 1.5 );
		sobel1.getParam( "normalizedKernel" ).set( false );
		sobel1.getParam( "computeGradientDirection" ).set( false );
		sobel1.getParam( "kernelEpsilon" ).set( 0.01 );
		sobel1.getParam( "pass" ).set( 1 );
		sobel2.getParam( "border" ).set( 3 );
		sobel2.getParam( "size" ).set( 1.5, 1.5 );
		sobel2.getParam( "normalizedKernel" ).set( false );
		sobel2.getParam( "computeGradientDirection" ).set( false );
		sobel2.getParam( "kernelEpsilon" ).set( 0.01 );
		sobel2.getParam( "pass" ).set( 2 );
//		normalize.getParam( "mode" ).set( 0 ); //"analyse" );
//		normalize.getParam( "analyseMode" ).set( 0 ); //"perChannel" );
//		normalize.getParam( "processR" ).set( false );
//		normalize.getParam( "processG" ).set( false );
//		normalize.getParam( "processB" ).set( true  );
//		normalize.getParam( "processA" ).set( false );
		floodfill.getParam( "upperThres" ).set( 0.1 );
		floodfill.getParam( "lowerThres" ).set( 0.025 );
//		canny.getParam( "fillAllChannels" ).set( true );

		write1.getParam( "components" ).set( 1 );
		write2.getParam( "components" ).set( 1 );
		write2.getParam( "components" ).set( 1 );
		write3.getParam( "components" ).set( 1 );
		
		write1.getParam( "filename" ).set( "data/canny/0_sobel.png" );
		write2.getParam( "filename" ).set( "data/canny/1_localMaxima.png" );
		write3.getParam( "filename" ).set( "data/canny/2_floodfill.png" );
		write4.getParam( "filename" ).set( "data/canny/3_thinning.png" );
		
		TCOUT( "__________________________________________________4" );
		g.connect( inputBuffer1, blur );
		g.connect( blur, sobel1 );
		g.connect( sobel1, sobel2 );
//		g.connect( read1, bitdepth );
//		g.connect( bitdepth, sobel1 );
		g.connect( sobel2, localmaxima );
		g.connect( localmaxima, floodfill );
		g.connect( floodfill, thinning );

		g.connect( sobel1, write1 );
		g.connect( localmaxima, write2 );
		g.connect( floodfill, write3 );
		g.connect( thinning, write4 );

		TCOUT( "__________________________________________________5" );
		std::list<std::string> outputs;
//		outputs.push_back( write1.getName() );
//		outputs.push_back( write2.getName() );
//		outputs.push_back( write2.getName() );
//		outputs.push_back( write3.getName() );
//		outputs.push_back( write4.getName() );
		outputs.push_back( thinning.getName() );

		boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
		memory::MemoryCache res0 = g.compute( thinning, 0 );
//		memory::MemoryCache res = g.compute( outputs, 0 );
		boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

		boost::posix_time::ptime t1b(boost::posix_time::microsec_clock::local_time());
		memory::MemoryCache res1 = g.compute( thinning, 0, 9 );
		boost::posix_time::ptime t2b(boost::posix_time::microsec_clock::local_time());

		COUT( "Process 0 took: " << t2a - t1a );
		COUT( "Process 1 took: " << (t2b - t1b)/10.0 );

		std::cout << res0 << std::endl;
		memory::CACHE_ELEMENT imgRes = res0.get( thinning.getName(), 0 );

		TCOUT_VAR( imgRes->getROD() );
		TCOUT_VAR( imgRes->getBounds() );
		boost::gil::rgba32f_view_t imgResView = imgRes->getGilView<boost::gil::rgba32f_view_t>();
		boost::gil::png_write_view( "data/canny/manual_output.png", boost::gil::color_converted_view<boost::gil::rgba8_pixel_t>( imgResView ) );
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

