#define WITHOUT_OFX

#include <tuttle/plugin/utils/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ofx/ofxsGil.hpp>
#include <tuttle/plugin/ofx/Progress.hpp>

#include <lensDistortResample.hpp>

#include <string>
#include <sstream>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>

using namespace boost::gil;
using namespace tuttle::plugin::lens;
typedef double F;

int main( int argc, char** argv )
{
	if( argc <= 2 )
	{
		TUTTLE_COUT_FATALERROR( "need one argument (image filename) : " << argv[0] << " image.jpg 0.5" );
		return 1;
	}
	std::string path( argv[1] );
	std::string ext( path, path.rfind( '.' ) + 1, 3 );

	TUTTLE_COUT_VAR( path );
	TUTTLE_COUT_VAR( ext );

	boost::gil::rgba8_image_t img_read;
	/*
	   if( ext == "jpg" )
	    jpeg_read_image( path.c_str(), img_read );
	   else*/if( ext == "png" )
		png_read_image( path.c_str(), img_read );
	/*else if( ext == "tif" )
	    tiff_read_image( path.c_str(), img_read );*/
	else
	{
		TUTTLE_COUT_FATALERROR( "Image file extension not recognize : " << ext );
		return 1;
	}

	std::stringstream ss( argv[2] ); // Could of course also have done ss("1234") directly.
	double coef1;

	if( ( ss >> coef1 ).fail() )
	{
		TUTTLE_COUT_FATALERROR( "Value not recognize : " << argv[2] );
		return 1;
	}
	TUTTLE_COUT_VAR( coef1 );

	boost::gil::rgb32f_image_t img_src( img_read.dimensions() );
	boost::gil::rgb32f_image_t img_dst( img_read.dimensions() );

	copy_and_convert_pixels( const_view( img_read ), view( img_src ) );
	//	copy_and_convert_pixels( const_view(img_read), view(img_dst) );

	//    typedef mpl::vector<rgb32_image_t> my_img_types;
	//	any_image<my_img_types> img;
	//	jpeg_read_image("input.jpg", img);

	//png_write_view( "output_crop.png", color_converted_view<rgb8_pixel_t>( subimage_view(const_view(img_src), -250, -558, img_src.dimensions().x+800, img_src.dimensions().y+1500 ) ) );

	fill_pixels( view( img_dst ), rgb32f_pixel_t( 0.8, 0.0, 0.0 ) );

	tuttle::plugin::lens::AdvancedLensDistortParams<double> _p;
	/*
	   point2<int> imgDataSize = point2<int>( img_src.dimensions().x, img_src.dimensions().y );
	   _p._dstDataSize = point2<int>( imgDataSize );
	   _p._imgSize = point2<double>( imgDataSize.x, imgDataSize.y );
	   _p._imgCenter = point2<double>( _p._imgSize.x / 2.0, _p._imgSize.y / 2.0 );
	   _p._imgDiagonal = std::sqrt( _p._imgSize.x * _p._imgSize.x + _p._imgSize.y * _p._imgSize.y );
	   _p._pixelRatio = 1.0;
	   _p._pixelRatioInv = 1.0 / _p._pixelRatio;


	   int _lensType = 0;
	   int _interpolation = 1;
	   _p._coef1 = coef1;
	   _p._coef2 = 0.0;
	   _p._squeeze = 1;
	   _p._asymmetric = point2<double>( 0.0, 0.0);
	   _p._lensCenter = _p.centerNormalizedToPixel( point2<double>( 0.0, 0.0) );
	   _p._scale = 1.0;
	 */
	//--------------------------------------------------------------------//
	ELensType _lensType           = eNormalLens;
	EInterpolation _interpolation = eBilinear;
	_p._coef1 = coef1;
	if( _p._coef1 >= 0 )
		_p._distort = true;     // distort
	else
		_p._distort = false;     // undistort
	_p._coef1      = std::abs( _p._coef1 );
	_p._coef2      = 0;
	_p._squeeze    = 0;
	_p._asymmetric = point2<double>( 0.0, 0.0 );
	_p._scale      = 1.0;
	if( _p._scale != 0 )
		_p._scale = 1.0 / _p._scale;
	point2<F> imgShift = point2<F>( 0.0, 0.0 );
	point2<F> imgSize  = point2<F>( img_src.dimensions().x, img_src.dimensions().y );
	_p._imgCenterSrc  = point2<F>( imgSize.x * 0.5, imgSize.y * 0.5 );
	_p._imgCenterDst  = point2<F>( imgSize.x * 0.5, imgSize.y * 0.5 ) + imgShift;
	_p._imgDiagonal   = std::sqrt( _p._imgCenterSrc.x * _p._imgCenterSrc.x + _p._imgCenterSrc.y * _p._imgCenterSrc.y );
	_p._pixelRatio    = 1;
	_p._lensCenterDst = point2<double>( 0.5, 0.5 ) * imgSize;
	_p._lensCenterSrc = _p._lensCenterDst + imgShift;

	TUTTLE_COUT( "---" );
	TUTTLE_COUT_VAR2( _p._lensCenterSrc.x, _p._lensCenterSrc.y );
	TUTTLE_COUT_VAR2( _p._lensCenterDst.x, _p._lensCenterDst.y );
	TUTTLE_COUT_VAR2( _p._imgCenterSrc.x, _p._imgCenterSrc.y );
	TUTTLE_COUT_VAR2( _p._imgCenterDst.x, _p._imgCenterDst.y );
	TUTTLE_COUT_VAR( _p._imgDiagonal );
	TUTTLE_COUT_VAR( _p._pixelRatio );

	ofx::Progress progress;
	OfxRectI procWindow;
	procWindow.x1 = 0;
	procWindow.y1 = 0;
	procWindow.x2 = imgSize.x;
	procWindow.y2 = imgSize.y;
	tuttle::plugin::lens::resample_pixels<bilinear_sampler>( const_view( img_src ), view( img_dst ), static_cast<tuttle::plugin::lens::NormalLensDistortParams<double>*>( &_p ), procWindow, &progress );

	png_write_view( "output_src.png", color_converted_view<rgb8_pixel_t>( const_view( img_src ) ) );
	png_write_view( "output_dst.png", color_converted_view<rgb8_pixel_t>( const_view( img_dst ) ) );
	tiff_write_view( "output_dst.tiff", color_converted_view<rgb8_pixel_t>( const_view( img_dst ) ) );

}

