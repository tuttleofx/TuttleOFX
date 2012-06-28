#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image_view_factory.hpp>

int main( int argc, char** argv )
{
	try
	{
		using namespace tuttle::host;

		Core::instance().preload();

		boost::gil::rgba8_image_t imgRead;
		boost::gil::png_read_and_convert_image( "in.png", imgRead );
		boost::gil::rgba8_view_t imgView( view( imgRead ) );

		Graph g;
		InputBufferNode& inputBuffer1 = g.createInputBuffer();
		Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

		// setup the input buffer
		const OfxRectD ibRod = { 0, 0, imgView.width(), imgView.height() };
		inputBuffer1.setRawImageBuffer(
				reinterpret_cast<char*>( boost::gil::interleaved_view_get_raw_data( imgView ) ),
				ibRod,
				ofx::imageEffect::ePixelComponentRGBA,
				ofx::imageEffect::eBitDepthUByte,
				imgView.pixels().row_size(),
				attribute::Image::eImageOrientationFromBottomToTop
			);
		
		write1.getParam( "filename" ).setValue( "out.png" );

		g.connect( inputBuffer1, write1 );
		g.compute( write1 );
	}
	catch( tuttle::exception::Common& e )
	{
		std::cout << "Tuttle Exception" << std::endl;
		std::cerr << boost::diagnostic_information( e );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information();

	}

	return 0;
}

