
namespace tuttle {
namespace plugin {
namespace debugImageEffectApi {

template<class View>
DebugImageEffectApiProcess<View>::DebugImageEffectApiProcess( DebugImageEffectApiPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View>
void DebugImageEffectApiProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup(args);
	_plugin._currentTime->setValue( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void DebugImageEffectApiProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	
	View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	copy_pixels( src, dst );
}

}
}
}
