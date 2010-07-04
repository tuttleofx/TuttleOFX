#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderProcess.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <OpenImageIO/imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;


template<class View>
OpenImageIOReaderProcess<View>::OpenImageIOReaderProcess( OpenImageIOReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void OpenImageIOReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	OpenImageIOReaderProcessParams params = _plugin.getProcessParams(args.time);
	if( ! bfs::exists( params._filepath ) )
	{
		throw( OFX::Exception::Suite(kOfxStatFailed, std::string("Unable to open : ") + params._filepath ) );
	}

	ImageGilProcessor<View>::setup( args );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void OpenImageIOReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	readImage( this->_dstView, _plugin.getProcessParams(this->_renderArgs.time)._filepath );
}

/**
 */
template<class View>
View& OpenImageIOReaderProcess<View>::readImage( View& dst, const std::string& filepath )
{
	using namespace OpenImageIO;
	boost::scoped_ptr<ImageInput> in( ImageInput::create( filepath ) );
	if( !in )
		throw OFX::Exception::Suite( kOfxStatErrValue );
	ImageSpec spec;
	in->open( filepath, spec );
	in->read_image( TypeDesc::UINT32, &((*dst.begin())[0]) ); // get the adress of the first channel value from the first pixel
	in->close();

	return dst;
}

}
}
}
}
