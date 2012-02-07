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


OCIOLutPlugin* ocioLutPlugin;

template<class View>
void OCIOLutProcess<View>::setup( const OFX::RenderArguments& args )
{
	ocioLutPlugin = &_plugin;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

//	switch( _params._inputType )
//	{
//		case eParamChooseInputCode:
//		{
////			TUTTLE_COUT_ERROR( "NotImplemented. Can't load text value." );
////			TUTTLE_COUT( "CTL -- Load code: " << _params._code );
//			loadModule( _interpreter, _params._module, _params._code );
//		}
//		case eParamChooseInputFile:
//		{
//			_interpreter.setModulePaths( _params._paths );
////			TUTTLE_COUT( "CTL -- Load module: " << _params._module );
//			_interpreter.loadModule( _params._module );
//		}
//	}
	//TODO
	//Ctl::setMessageOutputFunction( ctlMessageOutput );
}


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


	copy_pixels( src, dst );

	try
	{
		// Load the current config.
		OCIO::ConstConfigRcPtr config = _params._config;

		// Get the processor
		OCIO::ConstProcessorRcPtr processor = config->getProcessor( inputspace.c_str(), outputspace.c_str());

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
