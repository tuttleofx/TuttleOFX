#include "ViewerAlgorithm.hpp"

#include <caca.h>
#include <boost/gil/extension/numeric/pixel_by_channel.hpp>


namespace tuttle {
namespace plugin {
namespace viewer {



template<class View>
ViewerProcess<View>::ViewerProcess( ViewerPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
    this->setNoMultiThreading();
}

template<class View>
void ViewerProcess<View>::setup( const OFX::RenderArguments& args )
{
    ImageGilFilterProcessor<View>::setup( args );
    _params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ViewerProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
        using namespace boost::gil;
        //const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );

        View src = this->_srcView;
        View dst = this->_dstView;

        copy_pixels( src, dst );

        size_t width    = src.width();
        size_t height   = src.height();
        size_t channels = 4;

        openGLWindow( width, height );


/*
        char* data = new char[ width * height * channels ];

        for( size_t i = 0; i < height; i++ )
            for( size_t j = 0; j < width; j++ )
            {
                data[ j * 3 + i * width * 3 + 0 ] = 128;
                data[ j * 3 + i * width * 3 + 1 ] = 128;
                data[ j * 3 + i * width * 3 + 2 ] = 128;
            }*/
/*
        float* data = new float[ width * height * channels ];

        for( size_t i = 0; i < height; i++ )
            for( size_t j = 0; j < width; j++ )
            {
                data[ j * channels + i * width * channels + 0 ] = 0.9;
                data[ j * channels + i * width * channels + 1 ] = 0.0;
                data[ j * channels + i * width * channels + 2 ] = 0.5;
            }
*/

        unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );//reinterpret_cast<float*>( &src(0,0)[0] );

        loadNewUnsignedByteTexture( dataPtr, width, height, channels );
        //loadNewShortTexture( dataPtr, width, height, channels );
        //loadNewFloatTexture( dataPtr, width, height, channels );

        // boucles
        glutIdleFunc(idle);
        glutMainLoop();
}

}
}
}
