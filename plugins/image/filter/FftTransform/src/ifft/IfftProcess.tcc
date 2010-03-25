/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "IfftProcess.hpp"
#include "IfftDefinitions.hpp"
namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

template<class View>
IfftProcess<View>::IfftProcess( IfftPlugin &instance )
: ImageGilProcessor<View>( instance )
, _plugin( instance )
{
    _srcClipRe = instance.fetchClip( kSourcePhase );
    _srcClipIm = instance.fetchClip( kSourceModule );
	_dstClip = instance.fetchClip( kOfxImageEffectOutputClipName );
	this->setNoMultiThreading();
}

template <class View>
void IfftProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;

	// destination view
	this->_dst.reset( _dstClip->fetchImage( args.time ) );
	if( !this->_dst.get( ) )
	    throw( ImageNotReadyException( ) );
	if( this->_dst->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_dstView = this->getView( this->_dst.get(), _dstClip->getPixelRod(args.time) );

	// Re source view
	this->_srcMod.reset( _srcClipRe->fetchImage( args.time ) );
	if( !this->_srcMod.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_srcMod->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewRe = this->getView( this->_srcMod.get(), _srcClipRe->getPixelRod(args.time) );

	// Make sure bit depths are same
	if( this->_srcMod->getPixelDepth() != this->_dst->getPixelDepth() ||
	    this->_srcMod->getPixelComponents() != this->_dst->getPixelComponents() )
	    throw( BitDepthMismatchException( ) );

	// Im source view
	this->_srcPhase.reset( _srcClipIm->fetchImage( args.time ) );
	if( !this->_srcPhase.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_srcPhase->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewIm = this->getView( this->_srcPhase.get(), _srcClipIm->getPixelRod(args.time) );

	// Make sure bit depths are same
	if( this->_srcPhase->getPixelDepth() != this->_dst->getPixelDepth() ||
	    this->_srcPhase->getPixelComponents() != this->_dst->getPixelComponents() )
	    throw( BitDepthMismatchException( ) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void IfftProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	
	View srcMod = subimage_view( this->_srcViewRe, procWindow.x1, procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	View srcPhase = subimage_view( this->_srcViewRe, procWindow.x1, procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );

	// Create a planar floating point view
//	typedef pixel<bits32f, layout<typename color_space_type<View>::type> > Pixel32f;
	typedef rgb32f_pixel_t Pixel32f;
	typedef typename boost::gil::image<Pixel32f, true> PlanarImage;
	typedef typename PlanarImage::view_t PlanarView;

	PlanarImage simgRe(srcMod.dimensions());
	PlanarView svwMod(view(simgRe));
	PlanarImage simgIm(srcPhase.dimensions());
	PlanarView svwPhase(view(simgIm));

	rgb32f_planar_image_t dimg(dst.dimensions());
	rgb32f_planar_view_t dvw(view(dimg));

	// Convert source view type to planar view
	copy_and_convert_pixels( srcMod, svwMod );
	copy_and_convert_pixels( srcPhase, svwPhase );
	// Apply fft on each plane
	tuttle::filter::fft::FftwWrapperCPU fft;

	for(int c = 0; c < num_channels<Pixel32f>::value; ++c)
	{
		fft.impfft( (const float*)boost::gil::planar_view_get_raw_data (svwMod, c),
				    (const float*)boost::gil::planar_view_get_raw_data (svwPhase, c),
				    (float*)boost::gil::planar_view_get_raw_data (dvw, c),
				    dst.width(), dst.height() );
	}

	// Convert planar view to dst view type
	copy_and_convert_pixels( dvw, dst );
}

}
}
}
}
