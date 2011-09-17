#ifndef OPENIMAGEIO_WRITER_PROCESS_HPP
#define OPENIMAGEIO_WRITER_PROCESS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class OpenImageIOWriterProcess : public ImageGilFilterProcessor<View>
{
	typedef OpenImageIOWriterProcess<View> This;
	
public:
	typedef typename image_from_view<View>::type Image;
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef typename View::point_t Point;
	typedef typename View::coord_t Coord;

protected:
	OpenImageIOWriterPlugin&    _plugin;        ///< Rendering plugin

public:
	OpenImageIOWriterProcess( OpenImageIOWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	void writeImage( const View& src, const std::string& filepath, const OpenImageIO::TypeDesc bitDepth );
	
	static bool progressCallback( void *opaque_data, float portion_done )
	{
		This* this_ptr = reinterpret_cast<This*>(opaque_data);
		return this_ptr->progressUpdate( portion_done );
	}
};

}
}
}
}

#include "OpenImageIOWriterProcess.tcc"

#endif
