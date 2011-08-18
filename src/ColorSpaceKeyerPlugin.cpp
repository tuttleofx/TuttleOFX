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
		_paramBoolDisplayGeodesicForm = fetchBooleanParam(kBoolOnlySelection);			//display geodesic form - check box
		_paramChoiceAverageMode = fetchChoiceParam(kColorAverageMode);					//average color mode - Choice param
		_paramRGBAColorSelection = fetchRGBAParam(kColorAverageSelection);				//average color selection - RGBA param
		
		//verify display Discrete enable value
		if(_paramBoolPointCloudDisplay->getValue())	//called default value
		{
			_paramBoolDiscretizationActive->setEnabled(true);	//Enable discretization check box
			_paramIntDiscretization->setEnabled(true);			//Enable discretization int param
		}
		//verify choice average enable value
		if(_paramChoiceAverageMode->getValue() == 1)
		{
			_paramRGBAColorSelection->setEnabled(true);			//Enable color average selection (RGBA param)
		}
		
		 _updateAverage = false;		//does display need to update average
		 _updateGeodesicForm = false;	//does display need to update geodesic form
		 _updateVBO = false;			//does display need to update VBO
		 _updateGeodesicFormAverage = false;	//does Geodesic form need to be updated
		 _resetViewParameters = false;	//do view parameters need to be reseted
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
			_updateGeodesicForm = true;
			if(_paramChoiceAverageMode->getValue() == 1)	//if average selection mode is manual
				_updateGeodesicFormAverage = true; //Modify geodesic form (with manual average)
		}
	}
	if( paramName == kPushButtonResetTransformationParameters) //Push button reset transformation has changed
	{
		 _resetViewParameters = true; //reset view parameters on overlay
	}
	if( paramName == kColorAverageMode)
	{
		if(_paramChoiceAverageMode->getValue() == 1)
		{
			_paramRGBAColorSelection->setEnabled(true); //activate color average selection
			_updateGeodesicFormAverage = true; //update geodesic form with average
		}
		else
			_paramRGBAColorSelection->setEnabled(false); //disable color average selection
		_updateGeodesicForm = true; //update geodesic form
	}
	if( paramName == kColorAverageSelection)
	{
		_updateGeodesicForm = true;	//update Geodesic Form
		_updateGeodesicFormAverage = true; //update Geodesic Form average
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
			OFX::Clip* test = fetchClip(clipName); //test clip
			if(test->isConnected())
			{
				getCloudPointData().generateVBOData(	//create a VBO data with discretization (and the new discretization step)
					_clipSrc,
					args.renderScale,
					_paramBoolDiscretizationActive->getValue(),
					_paramIntDiscretization->getValue() ); 
			
				_updateVBO = true;			//update VBO on overlay
				this->redrawOverlays();		//redraw scene
			}
			else //source clip is no more connected
			{
				getCloudPointData()._imgVBO.deleteVBO(); //delete VBO
			}
		}
	}
	else if( clipName == kClipColorSelection) // if color clip has changed
	{
		if( this->hasCloudPointData() ) //it is not batch mode
		{
			OFX::Clip* test = fetchClip(clipName); //test clip
			if(test->isConnected())
			{
				_updateAverage = true;		//update selection average
				_updateVBO = true;			//update VBO on overlay
				_updateGeodesicForm = true; //update Geodesic form
				this->redrawOverlays();		//redraw scene
			}
		}
	}
	else
	{
		
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
			
			_updateAverage = true; //color clip selection need to be updated in overlay
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
