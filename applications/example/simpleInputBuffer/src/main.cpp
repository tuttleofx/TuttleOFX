#include <tuttle/host/Graph.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <boost/lexical_cast.hpp>

namespace {
void* callbackImagePointer( OfxTime time, void* customData )
{
	return customData;
}
}


int main( int argc, char** argv )
{
	try
	{
		using namespace tuttle::host;

		core().preload();

		boost::gil::rgba8_image_t imgRead;
		boost::gil::png_read_and_convert_image( "in.png", imgRead );
		boost::gil::rgba8_view_t imgView( view( imgRead ) );

		Graph g;
		//Graph::Node& inputBuffer = g.createNode("tuttle.inputbuffer");
		InputBufferWrapper inputBuffer = g.createInputBuffer();
		Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );
		
		inputBuffer.setRawImageBuffer(
			(void*)boost::gil::interleaved_view_get_raw_data( imgView ),
			imgView.width(), imgView.height(),
			InputBufferWrapper::ePixelComponentRGBA,
			InputBufferWrapper::eBitDepthUByte
			);
		inputBuffer.setCallback( callbackImagePointer, boost::gil::interleaved_view_get_raw_data( imgView ) );
		
		write1.getParam( "filename" ).setValue( "out.png" );

		g.connect( inputBuffer.getNode(), write1 );
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

