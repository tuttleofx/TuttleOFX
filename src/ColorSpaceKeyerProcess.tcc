#include "ColorSpaceKeyerAlgorithm.hpp"

#include <tuttle/plugin/image/gil/algorithm.hpp>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

template<class View>
ColorSpaceKeyerProcess<View>::ColorSpaceKeyerProcess( ColorSpaceKeyerPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	//this->setNoMultiThreading();
}

template<class View>
void ColorSpaceKeyerProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
	
	std::cout << "debut render" << std::endl;
	//Create geodesic form
	SelectionAverage selectionAverage(_plugin._time);	//create selection

	if(_plugin._paramChoiceAverageMode->getValue() ==0) //average mode is automatic
	{
		std::cout << "[Process] Compute average selection" << std::endl;
		selectionAverage.computeAverageSelection(_plugin._clipColor,_plugin._renderScale); //compute average selection
		std::cout << "[Process] Modify geodesic form" << std::endl;
		_geodesicForm.subdiviseFaces(selectionAverage._averageValue, _plugin._paramIntDiscretization->getValue()); //create geodesic form
	}
	else //average mode is manual
	{
		Ofx3DPointD selectedAverage; //initialize average
		OfxRGBAColourD colorSelected =  _plugin._paramRGBAColorSelection->getValue(); //get selected color
		selectedAverage.x = colorSelected.r; //x == red
		selectedAverage.y = colorSelected.g; //y == green
		selectedAverage.z = colorSelected.b; //z == blue
		//compute geodesic form
		_geodesicForm.subdiviseFaces(selectedAverage, _plugin._paramIntDiscretization->getValue()); //create geodesic form
	}
	std::cout << "[Process] Extend geodesic form" << std::endl;
	//Extend geodesic form
	selectionAverage.extendGeodesicForm(_plugin._clipColor,_plugin._renderScale,_geodesicForm); //extends geodesic form
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ColorSpaceKeyerProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	
	// this->_renderArgs.time
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
        
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							                  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							                  procWindowSize.x, procWindowSize.y );
	
	std::cout << "[Process-multi] create functor" << std::endl;
    //Create and initialize functor 
	Compute_alpha_pixel funct(false,_geodesicForm); //Output is alpha
	std::cout << "[Process-multi] compute alpha mask" << std::endl;
	//this function is chose because of functor reference and not copy
	transform_pixels_progress(src,dst,funct,*this);
	std::cout << "[Process-multi] fin render"<< std::endl;
}

}
}
}
