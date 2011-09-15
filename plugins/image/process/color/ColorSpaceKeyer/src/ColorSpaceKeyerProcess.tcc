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
	
	//std::cout << "[Process] début render" << std::endl;
	//Create geodesic form
	SelectionAverage selectionAverage(_plugin._time);								//create selection
	//update color geodesic form
	_geodesicFormColor._scale = _plugin._paramDoubleScaleGF->getValue();			//set good scale to geodesic form
	_geodesicFormColor._tolerance = _plugin._paramDoubleToleranceGF->getValue();	//set good scale to geodesic form
	//update spill geodesic form
	_geodesicFormSpill._scale = _plugin._paramDoubleScaleGF->getValue();			//set good scale to geodesic form
	_geodesicFormSpill._tolerance = _plugin._paramDoubleToleranceGF->getValue();	//set good scale to geodesic form
	
	//std::cout << "[Process] initialize geodesic form" << std::endl;
	if(_plugin._paramChoiceAverageMode->getValue() ==0) //average mode is automatic
	{
		selectionAverage.computeAverageSelection(_plugin._clipColor,_plugin._renderScale); //compute average selection
		//subdivise geodesic forms
		_geodesicFormColor.subdiviseFaces(selectionAverage._averageValue, _plugin._paramIntDiscretization->getValue()); //create geodesic form
		_geodesicFormSpill.subdiviseFaces(selectionAverage._averageValue, _plugin._paramIntDiscretization->getValue()); //create geodesic form
	}
	else //average mode is manual
	{
		Ofx3DPointD selectedAverage; //initialize average
		OfxRGBAColourD colorSelected =  _plugin._paramRGBAColorSelection->getValue(); //get selected color
		selectedAverage.x = colorSelected.r; //x == red
		selectedAverage.y = colorSelected.g; //y == green
		selectedAverage.z = colorSelected.b; //z == blue
		//compute geodesic forms
		_geodesicFormColor.subdiviseFaces(selectedAverage, _plugin._paramIntDiscretization->getValue()); //create geodesic form
		_geodesicFormSpill.subdiviseFaces(selectedAverage, _plugin._paramIntDiscretization->getValue()); //create geodesic form
	}
	//std::cout << "[Process] extend geodesic form" << std::endl;
	//Extend geodesic forms
	selectionAverage.extendGeodesicForm(_plugin._clipColor,_plugin._renderScale,_geodesicFormColor); //extends geodesic form color
	_geodesicFormSpill.copyGeodesicForm(_geodesicFormColor);										 //extends geodesic form spill (color clip)
	selectionAverage.extendGeodesicForm(_plugin._clipSpill,_plugin._renderScale,_geodesicFormSpill); //extends geodesic form spill (spill takes account of spill clip)
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
		
	//std::cout << "[Process] create functor and process" << std::endl;
    //Create and initialize functor 
	Compute_alpha_pixel funct(false,_geodesicFormColor, _geodesicFormSpill); //Output is alpha
	//this function is chose because of functor reference and not copy
	transform_pixels_progress(src,dst,funct,*this);
	//std::cout <<"[Process] fin render" << std::endl;
}

}
}
}
