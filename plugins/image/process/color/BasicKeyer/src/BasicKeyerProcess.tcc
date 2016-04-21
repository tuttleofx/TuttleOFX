#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace basicKeyer
{

template <class View>
BasicKeyerProcess<View>::BasicKeyerProcess(BasicKeyerPlugin& instance)
    : Parent(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void BasicKeyerProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    Parent::setup(args);

    _params = _plugin.getProcessParams<View>();

    //	OfxRectD rod = _plugin._clipDst->getCanonicalRod( args.time );
    //	Point dims( rod.x2 - rod.x1, rod.y2 - rod.y1 );
    //	int yshift = boost::numeric_cast<int>(( dims.x - dims.y ) * 0.5);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void BasicKeyerProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename View::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            *dst_it = *src_it;
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
