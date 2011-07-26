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
}

ColorSpaceKeyerProcessParams<ColorSpaceKeyerPlugin::Scalar> ColorSpaceKeyerPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorSpaceKeyerProcessParams<Scalar> params;
	return params;
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
				getCloudPointData().generateVBO(_clipSrc,args.time,args.renderScale,true,_paramIntDiscretization->getValue()); //create a VBO with discretization
			}
			else	//discretization is no more active
			{
				getCloudPointData().generateVBO(_clipSrc,args.time,args.renderScale,false,0);	//create a normal VBO
			}
		}
	}
	if( paramName == kIntDiscretizationDisplay) //discretization int range value has changed
	{
		if(_paramBoolDiscretizationActive->getValue() && hasCloudPointData()) //discretization is actived
			getCloudPointData().generateVBO(_clipSrc,args.time,args.renderScale,true,_paramIntDiscretization->getValue()); //create a VBO with discretization
	}
}

//bool ColorSpaceKeyerPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	ColorSpaceKeyerProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void ColorSpaceKeyerPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	ColorSpaceKeyerProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

/*
 * the source clip has been changed
 */

/*
 * If clip has changed
 */
void ColorSpaceKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		if( this->hasCloudPointData() )
		{
			this->getCloudPointData().generateVBO( this->_clipSrc, args.time, args.renderScale,_paramBoolDiscretizationActive->getValue(), _paramIntDiscretization->getValue());
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
		_cloudPointData.reset(new CloudPointData(imgSize));				//Create data
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
