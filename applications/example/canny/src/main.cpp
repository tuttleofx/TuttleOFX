#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/gil/gil_all.hpp>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		std::cerr << "canny: missing operand." << std::endl;
		std::cerr << "Usage: ./canny image.png" << std::endl;
		return 1;
	}
	try
	{
		using namespace tuttle::host;
		TUTTLE_TCOUT( "__________________________________________________0" );
		Core::instance().preload();

		TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );

		TUTTLE_TCOUT( "__________________________________________________1" );

//		boost::gil::rgba32f_image_t imgRead;
//		boost::gil::rgba8_image_t imgRead;
		boost::gil::gray8_image_t imgRead;
		boost::gil::png_read_and_convert_image( argv[1], imgRead );
//		boost::gil::rgba32f_view_t imgView( view( imgRead ) );
//		boost::gil::rgba8_view_t imgView( view( imgRead ) );
		boost::gil::gray8_view_t imgView( view( imgRead ) );

		TUTTLE_TCOUT( "__________________________________________________2" );

		Graph g;
		InputBufferNode& inputBuffer1 = g.createInputBuffer();
		Graph::Node& bitdepth1    = g.createNode( "tuttle.bitdepth" );
		Graph::Node& bitdepth2    = g.createNode( "tuttle.bitdepth" );
		Graph::Node& blur         = g.createNode( "tuttle.blur" );
		Graph::Node& blur1        = g.createNode( "tuttle.blur" );
		Graph::Node& blur2        = g.createNode( "tuttle.blur" );
		Graph::Node& sobel        = g.createNode( "tuttle.sobel" );
		Graph::Node& sobel1       = g.createNode( "tuttle.sobel" );
		Graph::Node& sobel2       = g.createNode( "tuttle.sobel" );
		Graph::Node& localMaxima  = g.createNode( "tuttle.localmaxima" );
		Graph::Node& floodfill    = g.createNode( "tuttle.floodfill" );
		Graph::Node& thinning     = g.createNode( "tuttle.thinning" );
		Graph::Node& write00       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write0       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write1a       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write1b       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write2       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write3       = g.createNode( "tuttle.pngwriter" );
		Graph::Node& write4       = g.createNode( "tuttle.pngwriter" );

		TUTTLE_TCOUT( "__________________________________________________3" );

		const OfxRectD ibRod = { 0, 0, imgView.width(), imgView.height() };
		inputBuffer1.setRawImageBuffer(
				reinterpret_cast<char*>( boost::gil::interleaved_view_get_raw_data( imgView ) ),
				ibRod,
				ofx::imageEffect::ePixelComponentAlpha,
				ofx::imageEffect::eBitDepthUByte,
				imgView.pixels().row_size(),
				attribute::Image::eImageOrientationFromTopToBottom
			);

		// Setup parameters
		static const double kernelEpsilon = 0.01;

		TUTTLE_COUT_VAR( kernelEpsilon );

		bitdepth1.getParam( "outputBitDepth" ).setValue( "float" );
		bitdepth2.getParam( "outputBitDepth" ).setValue( "byte" );

		blur.getParam( "border" ).setValue( "Padded" );
		blur.getParam( "size" ).setValue( 1.0, 1.0 );
		blur.getParam( "normalizedKernel" ).setValue( false );
		blur.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );

		blur1.getParam( "border" ).setValue( "Padded" );
		blur1.getParam( "size" ).setValue( 1.0, 0.0 );
		blur1.getParam( "normalizedKernel" ).setValue( false );
		blur1.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );

		blur2.getParam( "border" ).setValue( "Padded" );
		blur2.getParam( "size" ).setValue( 0.0, 1.0 );
		blur2.getParam( "normalizedKernel" ).setValue( false );
		blur2.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );

		sobel.getParam( "border" ).setValue( "Padded" );
		sobel.getParam( "size" ).setValue( 1.0, 1.0 );
		sobel.getParam( "normalizedKernel" ).setValue( false );
		sobel.getParam( "computeGradientDirection" ).setValue( false );
		sobel.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );
//		sobel.getParam( "unidimensional" ).setValue( true );
		sobel.getParam( "outputComponent" ).setValue( "RGB" );

		sobel1.getParam( "border" ).setValue( "Padded" );
		sobel1.getParam( "size" ).setValue( 1.0, 1.0 );
		sobel1.getParam( "normalizedKernel" ).setValue( false );
		sobel1.getParam( "computeGradientDirection" ).setValue( false );
		sobel1.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );
		sobel1.getParam( "pass" ).setValue( 1 );
		sobel1.getParam( "outputComponent" ).setValue( "RGB" );

		sobel2.getParam( "border" ).setValue( "Padded" );
		sobel2.getParam( "size" ).setValue( 1.0, 1.0 );
		sobel2.getParam( "normalizedKernel" ).setValue( false );
		sobel2.getParam( "computeGradientDirection" ).setValue( false );
		sobel2.getParam( "kernelEpsilon" ).setValue( kernelEpsilon );
		sobel2.getParam( "pass" ).setValue( 2 );
		sobel2.getParam( "outputComponent" ).setValue( "RGB" );

		localMaxima.getParam( "outputComponent" ).setValue( "Alpha" );

//		normalize.getParam( "mode" ).setValue( 0 ); //"analyse" );
//		normalize.getParam( "analyseMode" ).setValue( 0 ); //"perChannel" );
//		normalize.getParam( "processR" ).setValue( false );
//		normalize.getParam( "processG" ).setValue( false );
//		normalize.getParam( "processB" ).setValue( true  );
//		normalize.getParam( "processA" ).setValue( false );
		floodfill.getParam( "upperThres" ).setValue( 0.1 );
		floodfill.getParam( "lowerThres" ).setValue( 0.025 );

		write00.getParam( "channel" ).setValue( "rgba" );
		write0.getParam( "channel" ).setValue( "rgba" );
		write1a.getParam( "channel" ).setValue( "rgba" );
		write1b.getParam( "channel" ).setValue( "rgba" );
		write2.getParam( "channel" ).setValue( "rgba" );
		write3.getParam( "channel" ).setValue( "rgba" );
		write4.getParam( "channel" ).setValue( "rgba" );

		write00.getParam( "filename" ).setValue( "data/canny/0_input.png" );
		write0.getParam( "filename" ).setValue( "data/canny/0_blur.png" );
		write1a.getParam( "filename" ).setValue( "data/canny/1a_sobel.png" );
		write1b.getParam( "filename" ).setValue( "data/canny/1b_sobel.png" );
		write2.getParam( "filename" ).setValue( "data/canny/2_localMaxima.png" );
		write3.getParam( "filename" ).setValue( "data/canny/3_floodfill.png" );
		write4.getParam( "filename" ).setValue( "data/canny/4_thinning.png" );

		TUTTLE_TCOUT( "__________________________________________________4" );
//		g.connect( read1, bitdepth );
//		g.connect( bitdepth, sobel1 );

		g.connect( inputBuffer1, bitdepth1 );
		g.connect( bitdepth1, blur1 );
		g.connect( blur1, blur2 );
		g.connect( blur2, sobel1 );
		g.connect( sobel1, sobel2 );
		g.connect( sobel2, localMaxima );
//		g.connect( blur, sobel );
//		g.connect( sobel, localMaxima );
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
//		outputs.push_back( write0.getName() );
		outputs.push_back( write1a.getName() );
//		outputs.push_back( write1b.getName() );
		outputs.push_back( write2.getName() );
//		outputs.push_back( write2.getName() );
		outputs.push_back( write3.getName() );
		outputs.push_back( write4.getName() );
		outputs.push_back( bitdepth2.getName() );

		boost::posix_time::ptime t1(boost::posix_time::microsec_clock::local_time());
//		memory::MemoryCache res0 = g.compute( bitdepth2 );
		ComputeOptions options;
		options._returnBuffers = true;
		options._forceIdentityNodesProcess = true;
		memory::MemoryCache res0 = g.compute( outputs, options );
		boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());

		TUTTLE_COUT( "Process took: " << t2 - t1 );

		std::cout << res0 << std::endl;
		memory::CACHE_ELEMENT imgRes = res0.get( bitdepth2.getName(), 0 );

		TUTTLE_COUT_VAR( imgRes->getROD() );
		TUTTLE_COUT_VAR( imgRes->getBounds() );
//		boost::gil::rgba32f_view_t imgResView = imgRes->getGilView<boost::gil::rgba32f_view_t>();
//		boost::gil::rgba8_view_t imgResView = imgRes->getGilView<boost::gil::rgba8_view_t>();
		boost::gil::gray8_view_t imgResView = imgRes->getGilView<boost::gil::gray8_view_t>( tuttle::host::attribute::Image::eImageOrientationFromTopToBottom );
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

