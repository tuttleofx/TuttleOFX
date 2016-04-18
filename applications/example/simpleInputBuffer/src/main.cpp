#define int_p_NULL (int*) NULL
// Should be included first to avoid setjmp.h include troubles
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/lexical_cast.hpp>

namespace
{
typedef void* CustomDataPtr;
void callbackImagePointer(OfxTime time, CustomDataPtr customData, void** outRawdata, int* outWidth, int* outHeight,
                          int* outRowSizeBytes)
{
    boost::gil::rgba8_image_t& imgRead = *(boost::gil::rgba8_image_t*)customData;
    boost::gil::png_read_and_convert_image("in.png", imgRead);
    boost::gil::rgba8_view_t imgView(view(imgRead));

    *outRawdata = boost::gil::interleaved_view_get_raw_data(imgView);
    *outWidth = imgView.width();
    *outHeight = imgView.height();
    *outRowSizeBytes = imgView.pixels().row_size();
}
}

int main(int argc, char** argv)
{
    try
    {
        using namespace tuttle::host;

        core().preload();

        boost::gil::rgba8_image_t imgRead;

        Graph g;
        // Graph::Node& inputBuffer = g.createNode("tuttle.inputbuffer");
        InputBufferWrapper inputBuffer = g.createInputBuffer();
        Graph::Node& write1 = g.createNode("tuttle.pngwriter");

        if(false)
        {
            // set image buffer
            boost::gil::png_read_and_convert_image("in.png", imgRead);
            boost::gil::rgba8_view_t imgView(view(imgRead));
            inputBuffer.setRawImageBuffer((void*)boost::gil::interleaved_view_get_raw_data(imgView), imgView.width(),
                                          imgView.height(), InputBufferWrapper::ePixelComponentRGBA,
                                          InputBufferWrapper::eBitDepthUByte);
        }
        else
        {
            // set callback
            inputBuffer.setComponents(InputBufferWrapper::ePixelComponentRGBA);
            inputBuffer.setBitDepth(InputBufferWrapper::eBitDepthUByte);
            inputBuffer.setCallback(callbackImagePointer, &imgRead);
        }

        write1.getParam("filename").setValue("out.png");

        g.connect(inputBuffer.getNode(), write1);
        g.compute(write1);
    }
    catch(tuttle::exception::Common& e)
    {
        std::cout << "Tuttle Exception" << std::endl;
        std::cerr << boost::diagnostic_information(e);
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information();
    }

    return 0;
}