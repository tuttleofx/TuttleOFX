#include "ColorSpaceKeyerPlugin.hpp"
#include "ColorSpaceKeyerProcess.hpp"
#include "ColorSpaceKeyerDefinitions.hpp"
#include "CloudPointData.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {


ColorSpaceKeyerPlugin::ColorSpaceKeyerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
		_clipColor = fetchClip( kClipColorSelection );

		_paramBoolPointCloudDisplay = fetchBooleanParam( kPointCloudDisplay ); //Is CloudPointData displayed (boolean param)
		_cloudPointDataCount = 0;
		
		//Associate intern params pointers to GUI components
		_paramBoolDiscretizationActive = fetchBooleanParam(kBoolDiscretizationDisplay);	//is discretization active on point cloud - check box
		_paramIntDiscretization = fetchIntParam(kIntDiscretizationDisplay);				//discretization step - Int param
		 _paramIntNbOfDivisionsGF = fetchIntParam(kIntNumberOfDivisonGeodesicForm);		//number of divisions geodesic form - Int param
		
		//verify display Discrete enable value
		if(_paramBoolPointCloudDisplay->getValue())	//called default value
		{
			_paramBoolDiscretizationActive->setEnabled(true);	//Enable discretization check box
			_paramIntDiscretization->setEnabled(true);			//Enable discretization int param
		}
		
		 _updateVBO = false;	//does display need to update VBO
}

ColorSpaceKeyerProcessParams<ColorSpaceKeyerPlugin::Scalar> ColorSpaceKeyerPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorSpaceKeyerProcessParams<Scalar> params;	// create parameters container object
	return params;									// pass parameters to process
}

/**
 * A GUI parameter has changed
 * @param args		current time and renderScale
 * @param paramName parameter name (variable name)
 */
void ColorSpaceKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kPointCloudDisplay) //display point cloud check box value has changed
	{
		if(_paramBoolPointCloudDisplay->getValue())	//display point cloud is selected
		{	
			_paramBoolDiscretizationActive->setEnabled(true);	//Enable discretization check box
			_paramIntDiscretization->setEnabled(true);			//Enable discretization int param
		}
		else	//display point cloud is not selected
		{
			_paramBoolDiscretizationActive->setEnabled(false);	//discretization treatment
			_paramIntDiscretization->setEnabled(false);			// Disable discretization choice
		}
	}
	if( paramName == kBoolDiscretizationDisplay) // discretization active check boxe changed
	{
		if(hasCloudPointData())
		{
			if(_paramBoolDiscretizationActive->getValue()) //discretization is active now
			{
					getCloudPointData().generateVBOData(	//create a VBO data using discretization
					_clipSrc,
					args.renderScale,
					true,
					_paramIntDiscretization->getValue() ); 
				 _updateVBO = true;	//update VBO on overlay
			}
			else	//discretization is no more active
			{
					getCloudPointData().generateVBOData(	//create a VBO data without discretization
					_clipSrc,
					args.renderScale,
					false,
					_paramIntDiscretization->getValue() ); 
				  _updateVBO = true;	// update VBO on overlay
			}
		}
	}
	if( paramName == kIntDiscretizationDisplay) //discretization value has changed (int range)
	{
		if(hasCloudPointData()) //it is not batch mode
		{
			if(_paramBoolDiscretizationActive->getValue()) //discretization is actived
			{
					getCloudPointData().generateVBOData(	//create a VBO data with discretization (and the new discretization step)
					_clipSrc,
					args.renderScale,
					true,
					_paramIntDiscretization->getValue() ); 
					
					_updateVBO = true; //update VBO on overlay
			}
		}
	}
	if( paramName == kIntNumberOfDivisonGeodesicForm) //number of divisions geodesic form has changed (int range)
	{
		if(hasCloudPointData()) ///@todo : to remove
		{
			getCloudPointData()._geodesicForm.subdiviseFaces(getCloudPointData()._averageColor,_paramIntNbOfDivisionsGF->getValue()); //change value of subdivision
		}
	}
}

/*
 * If clip has changed
 */
void ColorSpaceKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName ) // if source clip has changed
	{
		if( this->hasCloudPointData() ) //it is not batch mode
		{
			getCloudPointData().generateVBOData(	//create a VBO data with discretization (and the new discretization step)
			_clipSrc,
			args.renderScale,
			_paramBoolDiscretizationActive->getValue(),
			_paramIntDiscretization->getValue() ); 
			
			 _updateVBO = true;		//update VBO on overlay
			this->redrawOverlays();		//redraw scene
		}
	}
	
	if( clipName == kClipColorSelection) // if color clip has changed
	{
		if( this->hasCloudPointData() ) //it is not batch mode
		{
			getCloudPointData().generateAverageColorSelection(_clipColor,args.renderScale);	//update average
			 _updateVBO = true;			//update VBO on overlay
			this->redrawOverlays();		//redraw scene
			
			//recompute geodesic form
			getCloudPointData()._geodesicForm.subdiviseFaces(getCloudPointData()._averageColor,_paramIntNbOfDivisionsGF->getValue()); //change value of subdivision
		}
	}
}

bool ColorSpaceKeyerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	ColorSpaceKeyerProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorSpaceKeyerPlugin::render( const OFX::RenderArguments &args )
{
	if(hasCloudPointData()) // if there is overlay data
	{
		if(args.time != getCloudPointData()._time) // different time between overlay and process
		{
			getCloudPointData()._time = args.time; // change computing time in cloud point data
			
			getCloudPointData().generateVBOData(	//create a VBO data (with discretization or not)
			_clipSrc,
			args.renderScale,
			_paramBoolDiscretizationActive->getValue(),
			_paramIntDiscretization->getValue() );
			
			getCloudPointData().generateAverageColorSelection( //update average data
					_clipColor,
					args.renderScale);
			
			_updateVBO = true; //VBO need to be updated in overlay
		}
	}
	doGilRender<ColorSpaceKeyerProcess>( *this, args );
}

/// @brief Cloud point data
/// @{
//cloud point data management
//Add a reference to CloudPointData
void ColorSpaceKeyerPlugin::addRefCloudPointData()
{
	if( _cloudPointDataCount == 0 )	//no reference has been added yet
	{
		const OfxPointI imgSize = this->_clipSrc->getPixelRodSize( 0 ); ///@todo set the correct time !
		_cloudPointData.reset(new CloudPointData(imgSize,0));				//Create data
	}
	++_cloudPointDataCount;//increments number of reference
}
//remove a reference to CloudPointData
void ColorSpaceKeyerPlugin::releaseCloudPointData()
{
	--_cloudPointDataCount;		
	if(_cloudPointDataCount == 0)	//no more reference on CloudPointData
	{
		_cloudPointData.reset(NULL);	//reset data
	}
}
//Has CloudPointData already been created (batch mode)
bool ColorSpaceKeyerPlugin::hasCloudPointData() const
{
	return _cloudPointDataCount != 0;
}

CloudPointData& ColorSpaceKeyerPlugin::getCloudPointData()
{
	return *_cloudPointData.get();
}
const CloudPointData& ColorSpaceKeyerPlugin::getCloudPointData() const
{
	return *_cloudPointData.get();
}
/// @}

}
}
}
