#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/numeric/rectOp.hpp>
#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/filter/thinning.hpp>

#include <boost/mpl/if.hpp>

namespace tuttle
{
namespace plugin
{
namespace thinning
{

template <class View>
ThinningProcess<View>::ThinningProcess(ThinningPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ThinningProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ThinningProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost;
    using namespace boost::gil;
    typedef typename View::point_t Point;
    typedef typename View::coord_t Coord;

    //	typedef typename mpl::if_c< num_channels<View>::value==1,
    //	                           View,
    //							   typename kth_channel_view_type<0,View>::type >::type CView;
    typedef View CView;
    typedef typename terry::image_from_view<CView, OfxAllocator<unsigned char> >::type CImage;

    static const std::size_t border = 1;
    const OfxRectI srcRodCrop1 = rectangleReduce(this->_srcPixelRod, border);
    const OfxRectI srcRodCrop2 = rectangleReduce(srcRodCrop1, border);
    const OfxRectI procWindowRoWCrop1 = rectanglesIntersection(rectangleGrow(procWindowRoW, border), srcRodCrop1);
    const OfxRectI procWindowRoWCrop2 = rectanglesIntersection(procWindowRoW, srcRodCrop2);

    //	TUTTLE_LOG_WARNING("-----");
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, this->_srcPixelRod );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, srcRodCrop1 );
    //	TUTTLE_LOG_INFO("");
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowRoW );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowRoWCrop1 );
    //	TUTTLE_LOG_WARNING("-----");

    /// @todo use an allocator
    OfxPointI tmpSize;
    tmpSize.x = procWindowRoWCrop1.x2 - procWindowRoWCrop1.x1;
    tmpSize.y = procWindowRoWCrop1.y2 - procWindowRoWCrop1.y1;
    CImage image_tmp(tmpSize.x, tmpSize.y);
    CView view_tmp = view(image_tmp);

    terry::algorithm::transform_pixels_locator_progress(
        this->_srcView, ofxToGil(this->_srcPixelRod), view_tmp, ofxToGil(procWindowRoWCrop1), ofxToGil(procWindowRoWCrop1),
        terry::filter::thinning::pixel_locator_thinning_t<View, CView>(this->_srcView, terry::filter::thinning::lutthin1),
        this->getOfxProgress());
    terry::algorithm::transform_pixels_locator_progress(
        view_tmp, ofxToGil(procWindowRoWCrop1), // srcRodCrop1,
        this->_dstView, ofxToGil(this->_dstPixelRod), ofxToGil(procWindowRoWCrop2),
        terry::filter::thinning::pixel_locator_thinning_t<CView, View>(view_tmp, terry::filter::thinning::lutthin2),
        this->getOfxProgress());
}
}
}
}
