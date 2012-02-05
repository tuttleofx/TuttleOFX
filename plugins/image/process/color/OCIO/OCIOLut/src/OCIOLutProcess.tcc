#if defined( _MSC_VER )
#pragma warning( disable : 4244 )
#endif

#include "OCIOLutProcess.hpp"
#include "OCIOLutDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <OpenColorIO/OpenColorIO.h>

#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace ocio {
namespace lut {

namespace OCIO = OCIO_NAMESPACE;

template<class View>
OCIOLutProcess<View>::OCIOLutProcess( OCIOLutPlugin& instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{ }

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void OCIOLutProcess<View>::multiThreadProcessImages(
													 const OfxRectI& procWindowRoW )
{
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );
	
	applyLut( dst, src );
}

template<class View>
void OCIOLutProcess<View>::applyLut(
		View& dst,
		View& src )
{
	using namespace boost::gil;
	typedef typename View::x_iterator vIterator;
	typedef typename channel_type<View>::type Pixel;

	static const char * inputcolorspace = "RawInput";
	static const char * outputcolorspace = "ProcessedOutput";

	copy_pixels( src, dst );

	try
	{
		// Load the current config.
		OCIO::ConstConfigRcPtr config = _plugin._config;

		// Get the processor
		OCIO::ConstProcessorRcPtr processor = config->getProcessor( inputcolorspace, outputcolorspace );

		if( is_planar<View>::value )
		{
			BOOST_THROW_EXCEPTION( exception::NotImplemented() );
		}
		else
		{
			/// @todo foreach line... to get progress information
			
			// Wrap the image in a light-weight ImageDescription
			OCIO::PackedImageDesc imageDesc( (float*)&( dst(0,0)[0] ), dst.width(), dst.height(), num_channels<View>::type::value, OCIO::AutoStride, dst.pixels().pixel_size(), dst.pixels().row_size() );

			// Apply the color transformation (in place)
			// Need normalized values
			processor->apply( imageDesc );
		}
	}
	catch( OCIO::Exception & exception )
	{
		TUTTLE_COUT(
					 tuttle::common::kColorError << "OCIO Error: "
					 << exception.what( ) << tuttle::common::kColorStd );
	}
	catch( ... )
	{
		TUTTLE_COUT(
					 tuttle::common::kColorError
					 << "Unknown OCIO error encountered."
					 << tuttle::common::kColorStd );
	}
}

}
}
}
}
