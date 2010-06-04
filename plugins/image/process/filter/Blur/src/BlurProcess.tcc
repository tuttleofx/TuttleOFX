#include "BlurPlugin.hpp"
#include "BlurAlgorithm.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/extension/numeric/convolve.hpp>
//#include <boost/gil/extension/io/png_dynamic_io.hpp> /// @todo tuttle: to remove

#include <boost/units/pow.hpp>
#include <boost/lambda/core.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <cmath>
#include <vector>
#include <algorithm>



namespace tuttle {
namespace plugin {
namespace blur {

template<class View>
BlurProcess<View>::BlurProcess( BlurPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template <class View>
void BlurProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void BlurProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	COUT_INFOS;
	using boost::numeric_cast;
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

//	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
//							  procWindowSize.x,
//							  procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );
	
	Point proc_tl( procWindowRoW.x1-this->_srcPixelRod.x1, procWindowRoW.y1-this->_srcPixelRod.y1 );

	COUT_X( 15, "-" );
	COUT_VAR( procWindowRoW );
	COUT_VAR( procWindowOutput );
	COUT_VAR( this->_srcPixelRod );
	COUT_VAR( this->_dstPixelRod );
	COUT_VAR( this->_src->getBounds() );
	COUT_VAR( this->_srcView.dimensions() );
	COUT_VAR( this->_dstView.dimensions() );
	COUT_VAR2( proc_tl.x, proc_tl.y );
	COUT_X( 15, "-" );
	
	convolve_boundary_option option = convolve_option_extend_mirror;
	switch( _params._border )
	{
		case eBorderExtendMirror:
			option = convolve_option_extend_mirror;
			break;
		case eBorderExtendConstant:
			option = convolve_option_extend_constant;
			break;
		case eBorderExtendBlack:
			option = convolve_option_extend_zero;
			break;
		case eBorderOutputBlack:
			option = convolve_option_output_zero;
			break;
	}

	if( _params._gilKernelX.size() > 2 && _params._gilKernelY.size() > 2 )
	{
		if( dst.dimensions() == this->_srcView.dimensions() ) // no tiles... easy !
		{
			COUT( "-- XY easy..." );
			convolve_rows<Pixel>( this->_srcView, _params._gilKernelX, dst, option );
			convolve_cols<Pixel>( dst, _params._gilKernelY, dst, option );
		}
		else
		{
			COUT( "-- XY with tiles !" );
			// we have 2 pass, so to use tiles, we need a temporary buffer
			// _________________src__________________
			// |    ____________dst____________      |
			// |    |  ....proc_src_roi.....   |     |
			// |    |  :  :             :  :   |     |
			// |    |  :  :_____________:  :   |     |
			// |    |  :  |             |  :   |     |
			// |    |  :  |   procWin   |  :   |     |
			// |    |  :  |             |  :   |     |
			// |    |  :  |_____________|  :   |     |
			// |    |  :  : tmp_buffer  :  :   |     |
			// |    |  :..:.............:..:   |     |
			// |    |__________________________|     |
			// |_____________________________________|
			// tmp_buffer: is the temporary buffer used after the convolve_rows
			//             (width of procWin and height of proc_src_roi)
			Coord top_in = std::min( numeric_cast<Coord>(_params._gilKernelY.left_size()), proc_tl.y );
			Coord bottom_in = std::min( numeric_cast<Coord>(_params._gilKernelY.right_size()), (this->_srcPixelRod.y2-this->_srcPixelRod.y1)-(proc_tl.y+procWindowSize.y) );
			Point image_tmp_size( dst.dimensions() );
			image_tmp_size.y += top_in + bottom_in;
//			image_tmp_size.y += _params._gilKernelY.size()-1;
			Point image_tmp_tl( proc_tl );
			image_tmp_tl.y -= top_in;

			Image image_tmp( image_tmp_size ); ///< @todo tuttle: use an allocator to use memory allocated from host
			View view_tmp = view( image_tmp );
			Point dst_tmp_tl( 0, top_in );

			COUT_VAR( top_in );
			COUT_VAR( _params._gilKernelY.left_size() );
			COUT_VAR( bottom_in );
			COUT_VAR( _params._gilKernelY.right_size() );
			COUT_VAR( this->_srcView.dimensions() );
			COUT_VAR( dst.dimensions() );
			COUT_VAR( view_tmp.dimensions() );
			COUT_VAR( image_tmp_tl );
			COUT_VAR( dst_tmp_tl );

			COUT_X(10, "_row_");
//			convolve_rows<Pixel>( this->_srcView, _params._gilKernelX, dst, proc_tl, option );
			convolve_rows<Pixel>( this->_srcView, _params._gilKernelX, view_tmp, image_tmp_tl, option );
			//COUT_X(10, "_save_row_");
			//png_write_view( "view_tmp.png", color_converted_view<rgba8_pixel_t>(view_tmp) );
			COUT_X(10, "_cols_");
			convolve_cols<Pixel>( view_tmp, _params._gilKernelY, dst, dst_tmp_tl, option );
			COUT_X(10, "_end_");
		}
	}
	else if( _params._gilKernelX.size() > 2 )
	{
		COUT( "-- X convolve" );
		COUT_X(10, "_row_");
		convolve_rows<Pixel>( this->_srcView, _params._gilKernelX, dst, proc_tl, option );
	}
	else if( _params._gilKernelY.size() > 2 )
	{
		COUT( "-- Y convolve" );
		COUT_X(10, "_cols_");
		convolve_cols<Pixel>( this->_srcView, _params._gilKernelY, dst, proc_tl, option );
	}
}

}
}
}
