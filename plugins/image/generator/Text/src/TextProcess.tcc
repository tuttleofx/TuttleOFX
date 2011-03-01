#include "TextPlugin.hpp"
#include "TextProcess.hpp"
#include "TextDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_inserter.hpp>
#include <boost/python.hpp>

#include <sstream>


namespace tuttle {
namespace plugin {
namespace text {

template<class View>
TextProcess<View>::TextProcess( TextPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
//	TUTTLE_TCOUT_INFOS;
//	Py_Initialize();
	TUTTLE_TCOUT_INFOS;
	this->setNoMultiThreading();
	TUTTLE_TCOUT_INFOS;
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

	
	if( ! _params._isExpression )
	{
		_text = _params._text;
	}
	else
	{
		try
		{
			TUTTLE_TCOUT_INFOS;
			Py_Initialize();
			
			TUTTLE_TCOUT_INFOS;
			boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__")))));
//			boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("toto")))));
			//boost::python::object main_module = boost::python::import( "__main__" );
			TUTTLE_TCOUT_INFOS;
			boost::python::object main_namespace = main_module.attr( "__dict__" );
			TUTTLE_TCOUT_INFOS;
			
			std::ostringstream context;
			context << "time = " << args.time << std::endl;
			context << "renderScale = [" << args.renderScale.x << "," << args.renderScale.y << "]" << std::endl;
			context << "renderWindow = [" << args.renderWindow.x1 << "," << args.renderWindow.y1 << ","
					                      << args.renderWindow.x2 << "," << args.renderWindow.y2 << "]" << std::endl;
			OfxRectD srcCanonicalRod = this->_clipSrc->getCanonicalRod( args.time );
			context << "srcCanonicalRod = [" << srcCanonicalRod.x1 << "," << srcCanonicalRod.y1 << ","
					                         << srcCanonicalRod.x2 << "," << srcCanonicalRod.y2 << "]" << std::endl;
			OfxRectI srcPixelRod = this->_clipSrc->getPixelRod( args.time );
			context << "srcPixelRod = [" << srcPixelRod.x1 << "," << srcPixelRod.y1 << ","
					                     << srcPixelRod.x2 << "," << srcPixelRod.y2 << "]" << std::endl;

			OfxRectD dstCanonicalRod = this->_clipDst->getCanonicalRod( args.time );
			context << "dstCanonicalRod = [" << dstCanonicalRod.x1 << "," << dstCanonicalRod.y1 << ","
					                         << dstCanonicalRod.x2 << "," << dstCanonicalRod.y2 << "]" << std::endl;
			OfxRectI dstPixelRod = this->_clipDst->getPixelRod( args.time );
			context << "dstPixelRod = [" << dstPixelRod.x1 << "," << dstPixelRod.y1 << ","
					                     << dstPixelRod.x2 << "," << dstPixelRod.y2 << "]" << std::endl;
			/*object ignored = */
			boost::python::exec( context.str().c_str(), main_namespace );
			boost::python::exec( _params._text.c_str(), main_namespace );

			_text = boost::python::extract<std::string>( main_namespace[kParamText.c_str()] );
			
//			object result = eval("5/0");
			// execution will never get here:
//			int five_divided_by_zero = extract<int>(result);
			TUTTLE_TCOUT_INFOS;
		}
		catch( boost::python::error_already_set const & )
		{
			// if we can't evaluate the expression
			// use the text without interpretation
			_text = _params._text;
		}
//		Py_Finalize();
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
	if( !this->_clipSrc->isConnected() )
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
	std::transform( _text.begin(), _text.end(), boost::ptr_container::ptr_back_inserter( _glyphs ), make_glyph( face ) );

	//Step 4. Make Metrics Array --------------------
	std::transform( _glyphs.begin(), _glyphs.end(), std::back_inserter( _metrics ), boost::gil::make_metric() );

	//Step 5. Make Kerning Array ----------------
	std::transform( _glyphs.begin(), _glyphs.end(), std::back_inserter( _kerning ), boost::gil::make_kerning() );

	//Step 6. Get Coordinates (x,y) ----------------
	_textSize.x   = std::for_each( _metrics.begin(), _metrics.end(), _kerning.begin(), boost::gil::make_width() );
	_textSize.y   = std::for_each( _metrics.begin(), _metrics.end(), boost::gil::make_height() );
	
	if( _metrics.size() > 1 )
		_textSize.x   += _params._letterSpacing * (_metrics.size() - 1);

	switch( _params._vAlign )
	{
		case eParamVAlignTop:
		{
			_textCorner.y = 0;
			break;
		}
		case eParamVAlignCenter:
		{
			_textCorner.y = ( this->_dstView.height() - _textSize.y ) * 0.5;
			break;
		}
		case eParamVAlignBottom:
		{
			_textCorner.y = this->_dstView.height() - _textSize.y - 1;
			break;
		}
	}
	switch( _params._hAlign )
	{
		case eParamHAlignLeft:
		{
			_textCorner.x = 0;
			break;
		}
		case eParamHAlignCenter:
		{
			_textCorner.x = ( this->_dstView.width() - _textSize.x ) * 0.5;
			break;
		}
		case eParamHAlignRight:
		{
			_textCorner.x = this->_dstView.width() - _textSize.x;
			break;
		}
	}

	if( _params._verticalFlip )
	{
		_dstViewForGlyphs = flipped_up_down_view( this->_dstView );
		_textCorner.y    -= _params._position.y;
	}
	else
	{
		_dstViewForGlyphs = this->_dstView;
		_textCorner.y    += _params._position.y;
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
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

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

	TUTTLE_TCOUT_INFOS;
	//Step 7. Render Glyphs ------------------------
	// if outside dstRod
	// ...
	// else
	View tmpDstViewForGlyphs = subimage_view( _dstViewForGlyphs, _textCorner.x, _textCorner.y, _textSize.x, _textSize.y );
	std::for_each( _glyphs.begin(), _glyphs.end(), _kerning.begin(),
	               render_glyph<View>( tmpDstViewForGlyphs, _foregroundColor, _params._letterSpacing )
	               );
	TUTTLE_TCOUT_INFOS;
}

}
}
}
