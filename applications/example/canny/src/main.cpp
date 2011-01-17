#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
		TUTTLE_TCOUT( "__________________________________________________0" );
//		Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
		// Core::instance().getPluginCache().scanPluginFiles();
		Core::instance().preload();

		TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );

		TUTTLE_TCOUT( "__________________________________________________1" );

//		boost::gil::rgba32f_image_t imgRead;
//		boost::gil::rgba8_image_t imgRead;
		boost::gil::gray8_image_t imgRead;
		boost::gil::png_read_and_convert_image( argv[1], //"data/input.png",
											    imgRead );
//		boost::gil::rgba32f_view_t imgView( view( imgRead ) );
//		boost::gil::rgba8_view_t imgView( view( imgRead ) );
		boost::gil::gray8_view_t imgView( view( imgRead ) );

		TUTTLE_TCOUT( "__________________________________________________2" );

		Graph g;
//		Graph::Node& inputBuffer1 = g.createNode( "fr.tuttle.inputbuffer" );
//		Graph::Node& read        = g.createNode( "fr.tuttle.pngreader" );
//		Graph::Node& bitdepth    = g.createNode( "fr.tuttle.bitdepth" );
		InputBufferNode& inputBuffer1 = g.createInputBuffer();
		Graph::Node& bitdepth1    = g.createNode( "fr.tuttle.bitdepth" );
		Graph::Node& bitdepth2    = g.createNode( "fr.tuttle.bitdepth" );
		Graph::Node& blur1        = g.createNode( "fr.tuttle.blur" );
		Graph::Node& blur2        = g.createNode( "fr.tuttle.blur" );
		Graph::Node& sobel1       = g.createNode( "fr.tuttle.duranduboi.sobel" );
		Graph::Node& sobel2       = g.createNode( "fr.tuttle.duranduboi.sobel" );
		Graph::Node& localMaxima  = g.createNode( "fr.tuttle.duranduboi.localmaxima" );
		Graph::Node& floodfill    = g.createNode( "fr.tuttle.duranduboi.floodfill" );
		Graph::Node& thinning     = g.createNode( "fr.tuttle.duranduboi.thinning" );
		Graph::Node& write00       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write0       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write1a       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write1b       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write2       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write3       = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write4       = g.createNode( "fr.tuttle.pngwriter" );

		TUTTLE_TCOUT( "__________________________________________________3" );

		OfxRectD ibRod = { 0, 0, imgView.width(), imgView.height() };
		inputBuffer1.setClipRod( ibRod );
		inputBuffer1.setClipBitDepth( InputBufferNode::eBitDepthUByte );
//		inputBuffer1.setClipBitDepth( InputBufferNode::eBitDepthFloat );
//		inputBuffer1.setClipComponent( InputBufferNode::ePixelComponentRGBA );
		inputBuffer1.setClipComponent( InputBufferNode::ePixelComponentAlpha );
		inputBuffer1.setClipRawBuffer( /*static_cast<char*>*/(char*)(boost::gil::interleaved_view_get_raw_data( imgView )) );

		// Setup parameters
		/*
		read1.getParam( "filename" ).set( "data/input1.avi" );
		read1.getParam( "filename" ).set( "data/input.png" );
		bitdepth.getParam( "outputBitDepth" ).set( 3 );
		*/

		bitdepth1.getParam( "outputBitDepth" ).set( "float" );
		bitdepth2.getParam( "outputBitDepth" ).set( "byte" );

		blur1.getParam( "border" ).set( "Padded" );
		blur1.getParam( "size" ).set( 1.0, 0.0 );
		blur1.getParam( "normalizedKernel" ).set( false );
		blur1.getParam( "kernelEpsilon" ).set( 0.1 );
		
		blur2.getParam( "border" ).set( "Padded" );
		blur2.getParam( "size" ).set( 0.0, 1.0 );
		blur2.getParam( "normalizedKernel" ).set( false );
		blur2.getParam( "kernelEpsilon" ).set( 0.1 );

		sobel1.getParam( "border" ).set( "Padded" );
		sobel1.getParam( "size" ).set( 1.0, 1.0 );
		sobel1.getParam( "normalizedKernel" ).set( false );
		sobel1.getParam( "computeGradientDirection" ).set( false );
		sobel1.getParam( "kernelEpsilon" ).set( 0.1 );
		sobel1.getParam( "pass" ).set( 1 );
		sobel1.getParam( "outputComponent" ).set( "RGB" );

		sobel2.getParam( "border" ).set( "Padded" );
		sobel2.getParam( "size" ).set( 1.0, 1.0 );
		sobel2.getParam( "normalizedKernel" ).set( false );
		sobel2.getParam( "computeGradientDirection" ).set( false );
		sobel2.getParam( "kernelEpsilon" ).set( 0.1 );
		sobel2.getParam( "pass" ).set( 2 );
		sobel2.getParam( "outputComponent" ).set( "RGB" );

		localMaxima.getParam( "outputComponent" ).set( "Alpha" );

//		normalize.getParam( "mode" ).set( 0 ); //"analyse" );
//		normalize.getParam( "analyseMode" ).set( 0 ); //"perChannel" );
//		normalize.getParam( "processR" ).set( false );
//		normalize.getParam( "processG" ).set( false );
//		normalize.getParam( "processB" ).set( true  );
//		normalize.getParam( "processA" ).set( false );
		floodfill.getParam( "upperThres" ).set( 0.1 );
		floodfill.getParam( "lowerThres" ).set( 0.025 );

		write00.getParam( "components" ).set( "rgba" );
		write0.getParam( "components" ).set( "rgba" );
		write1a.getParam( "components" ).set( "rgba" );
		write1b.getParam( "components" ).set( "rgba" );
		write2.getParam( "components" ).set( "rgba" );
		write2.getParam( "components" ).set( "rgba" );
		write3.getParam( "components" ).set( "rgba" );

		write00.getParam( "filename" ).set( "data/canny/0_input.png" );
		write0.getParam( "filename" ).set( "data/canny/0_blur.png" );
		write1a.getParam( "filename" ).set( "data/canny/1a_sobel.png" );
		write1b.getParam( "filename" ).set( "data/canny/1b_sobel.png" );
		write2.getParam( "filename" ).set( "data/canny/2_localMaxima.png" );
		write3.getParam( "filename" ).set( "data/canny/3_floodfill.png" );
		write4.getParam( "filename" ).set( "data/canny/4_thinning.png" );
		
		TUTTLE_TCOUT( "__________________________________________________4" );
//		g.connect( read1, bitdepth );
//		g.connect( bitdepth, sobel1 );
		g.connect( inputBuffer1, bitdepth1 );
		g.connect( bitdepth1, blur1 );
		g.connect( blur1, blur2 );
		g.connect( blur2, sobel1 );
		g.connect( sobel1, sobel2 );
		g.connect( sobel2, localMaxima );
		g.connect( localMaxima, floodfill );
		g.connect( floodfill, thinning );
		g.connect( thinning, bitdepth2 );

		g.connect( bitdepth1, write00 );
		g.connect( blur2, write0 );
		g.connect( sobel1, write1a );
		g.connect( sobel2, write1b );
		g.connect( localMaxima, write2 );
		g.connect( floodfill, write3 );
		g.connect( thinning, write4 );

		TUTTLE_TCOUT( "__________________________________________________5" );
		std::list<std::string> outputs;
		outputs.push_back( write00.getName() );
		outputs.push_back( write0.getName() );
		outputs.push_back( write1a.getName() );
		outputs.push_back( write1b.getName() );
		outputs.push_back( write2.getName() );
		outputs.push_back( write2.getName() );
		outputs.push_back( write3.getName() );
		outputs.push_back( write4.getName() );
		outputs.push_back( bitdepth2.getName() );

		boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
		memory::MemoryCache res0 = g.compute( bitdepth2, 0 );
//		memory::MemoryCache res0 = g.compute( outputs, 0 );
		boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

//		boost::posix_time::ptime t1b(boost::posix_time::microsec_clock::local_time());
//		memory::MemoryCache res1 = g.compute( thinning, 0, 9 );
//		boost::posix_time::ptime t2b(boost::posix_time::microsec_clock::local_time());

		TUTTLE_COUT( "Process 0 took: " << t2a - t1a );
//		TUTTLE_COUT( "Process 1 took: " << (t2b - t1b)/10.0 );

		std::cout << res0 << std::endl;
		memory::CACHE_ELEMENT imgRes = res0.get( bitdepth2.getName(), 0 );

		TUTTLE_COUT_VAR( imgRes->getROD() );
		TUTTLE_COUT_VAR( imgRes->getBounds() );
//		boost::gil::rgba32f_view_t imgResView = imgRes->getGilView<boost::gil::rgba32f_view_t>();
//		boost::gil::rgba8_view_t imgResView = imgRes->getGilView<boost::gil::rgba8_view_t>();
		boost::gil::gray8_view_t imgResView = imgRes->getGilView<boost::gil::gray8_view_t>();
		boost::gil::png_write_view( "data/canny/manual_output.png", boost::gil::color_converted_view<boost::gil::rgb8_pixel_t>( imgResView ) );
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

