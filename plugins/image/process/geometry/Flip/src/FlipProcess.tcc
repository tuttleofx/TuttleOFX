#include "FlipPlugin.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace flip {

template<class View>
FlipProcess<View>::FlipProcess( FlipPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
}

template<class View>
void FlipProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

        _params = _plugin.getProcessParams( args.time, args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void FlipProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
        using namespace boost::gil;

        OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
        OfxPointI procWindowSize  = {
                procWindowRoW.x2 - procWindowRoW.x1,
                procWindowRoW.y2 - procWindowRoW.y1
        };


        View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
                                                                          procWindowSize.x, procWindowSize.y );
        View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
                                                                          procWindowSize.x, procWindowSize.y );
        if( _params.flip )
        {
            if( _params.flop )
                copy_pixels( flipped_left_right_view ( flipped_up_down_view( src ) ), dst );
            else
                copy_pixels( flipped_up_down_view( src ), dst );
        }
        else
        {
            if( _params.flop )
                copy_pixels( flipped_left_right_view( src ), dst );
            else
                copy_pixels( src, dst );
        }




}

}
}
}
