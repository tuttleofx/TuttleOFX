#include "TextPlugin.hpp"
#include "TextProcess.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_inserter.hpp>

namespace tuttle {
namespace plugin {
namespace text {

template<class View>
TextProcess<View>::TextProcess( TextPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void TextProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams();

	if( !boost::filesystem::exists( _params._font ) )
	{
		return;
	}

	//Step 1. Create boost::gil image
	//Step 2. Initialize freetype
	//Step 3. Make Glyphs Array
	//Step 4. Make Metrics Array
	//Step 5. Make Kerning Array
	//Step 6. Get Coordinates (x,y)
	//Step 7. Render Glyphs on GIL View
	//Step 8. Save GIL Image

	//Step 1. Create boost::gil image -----------
	if( ! this->_clipSrc->isConnected() )
	{
//		rgba32f_pixel_t backgroundColor( params._backgroundColor.r,
//									     params._backgroundColor.g,
//									     params._backgroundColor.b,
//									     params._backgroundColor.a );
		rgba32f_pixel_t backgroundColor( 0, 0, 0, 0 );
		fill_pixels( this->_dstView, backgroundColor );
	}

	//Step 2. Initialize freetype ---------------
	FT_Library library;
	FT_Init_FreeType( &library );

	FT_Face face;
	FT_New_Face( library, _params._font.c_str(), 0, &face );
	FT_Set_Pixel_Sizes( face, _params._fontX, _params._fontY );

	//Step 3. Make Glyphs Array ------------------
	rgba32f_pixel_t rgba32f_foregroundColor( _params._fontColor.r,
									         _params._fontColor.g,
									         _params._fontColor.b,
									         _params._fontColor.a );
	color_convert( rgba32f_foregroundColor, _foregroundColor );
	std::transform( _params._text.begin(), _params._text.end(), boost::ptr_container::ptr_back_inserter( _glyphs ), make_glyph( face ) );

	//Step 4. Make Metrics Array --------------------
	std::transform( _glyphs.begin(), _glyphs.end(), std::back_inserter( _metrics ), boost::gil::make_metric() );

	//Step 5. Make Kerning Array ----------------
	std::transform( _glyphs.begin(), _glyphs.end(), std::back_inserter( _kerning ), boost::gil::make_kerning() );

	//Step 6. Get Coordinates (x,y) ----------------
	_textSize.x = std::for_each( _metrics.begin(), _metrics.end(), _kerning.begin(), boost::gil::make_width() );
	_textSize.y = std::for_each( _metrics.begin(), _metrics.end(), boost::gil::make_height() );
	_textCorner.x = ( this->_dstView.width() - _textSize.x ) * 0.5;
	_textCorner.y = ( this->_dstView.height() - _textSize.y ) * 0.5;

	if( _params._verticalFlip )
	{
		_dstViewForGlyphs = flipped_up_down_view( this->_dstView );
		_textCorner.y -= _params._position.y;
	}
	else
	{
		_dstViewForGlyphs = this->_dstView;
		_textCorner.y += _params._position.y;
	}
	_textCorner.x += _params._position.x;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void TextProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	
	for( int y = procWindowOutput.y1;
		 y < procWindowOutput.y2;
		 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
			 x < procWindowOutput.x2;
			 ++x, ++src_it, ++dst_it )
		{
			*dst_it = *src_it;
		}
		if( this->progressForward() )
			return;
	}

	//Step 7. Render Glyphs ------------------------
	View tmpDstViewForGlyphs = subimage_view( _dstViewForGlyphs, _textCorner.x, _textCorner.y, _textSize.x, _textSize.y );
	std::for_each( _glyphs.begin(), _glyphs.end(), _kerning.begin(),
				   render_glyph<View>( tmpDstViewForGlyphs, _foregroundColor, _params._letterSpacing )
				 );

}

}
}
}
