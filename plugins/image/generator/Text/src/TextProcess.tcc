
#define white 255
#define gray 170


#include "TextPlugin.hpp"
#include "TextProcess.hpp"
#define black 0

namespace tuttle {
namespace plugin {
namespace text {

template<class View>
TextProcess<View>::TextProcess( TextPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance ) { }

template<class View>
void TextProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	int fontW = 20; // fontSize
	int fontH = fontW * 1; // * ratio
	text_pixel_t backgroundColor( gray, gray, gray );
	text_pixel_t foregroundColor( white, black, black );

	std::string fontPath = "/usr/share/fonts/truetype/msttcorefonts/arial.ttf";

	//Step 1. Create bgil image
	//Step 2. Initialize freetype
	//Step 3. Make Glyphs Array
	//Step 4. Make Metrics Array
	//Step 5. Make Kerning Array
	//Step 6. Get Coordinates (x,y)
	//Step 7. Render Glyphs on GIL View
	//Step 8. Save GIL Image

	//Step 1. Create bgil image -----------
	//bgil::fill_pixels( bgil::view( this->_dstView ), backgroundColor );

	//Step 2. Initialize freetype ---------------

	FT_Library library;
	FT_Init_FreeType( &library );

	FT_Face face;
	FT_New_Face( library, fontPath.c_str(), 0, &face );
	FT_Set_Pixel_Sizes( face, fontW, fontH );

	//Step 3. Make Glyphs Array ------------------

	TextProcessParams params = _plugin.getProcessParams();
	std::transform( params._text.begin( ), params._text.end( ), std::back_inserter( _glyphs ), make_glyph( face, foregroundColor ) );

	//Step 4. Make Metrics Array --------------------

	std::transform( _glyphs.begin( ), _glyphs.end( ), std::back_inserter( _metrics ), bgil::make_metric( ) );

	//Step 5. Make Kerning Array ----------------

	std::transform( _glyphs.begin( ), _glyphs.end( ), std::back_inserter( _kerning ), bgil::make_kerning( ) );


}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void TextProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );

	/*
	View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	copy_pixels( src, dst );
	 */
	
	for( int y = procWindow.y1;
		 y < procWindow.y2;
		 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindow.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindow.x1, y );
		for( int x = procWindow.x1;
			 x < procWindow.x2;
			 ++x, ++src_it, ++dst_it )
		{
			( *dst_it ) = ( *src_it );
		}
		if( this->progressForward( ) )
			return;
	}
	

	//Step 6. Get Coordinates (x,y) ----------------
	int width = std::for_each( _metrics.begin( ), _metrics.end( ), _kerning.begin( ), bgil::make_width( ) );
	int height = std::for_each( _metrics.begin( ), _metrics.end( ), bgil::make_height( ) );
	int x = ( this->_dstView.width( ) - width ) / 2.0;
	int y = ( this->_dstView.height( ) - height ) / 2.0;

	x -= procWindow.x1;
	y -= procWindow.y1;
	
	//Step 7. Render Glyphs ------------------------

	std::for_each( _glyphs.begin( ), _glyphs.end( ), _kerning.begin( ),
				   bgil::render_gray_glyph<View>
				   (
				    bgil::subimage_view( dst, x, y, width, height )
				   )
				 );

}

}
}
}
