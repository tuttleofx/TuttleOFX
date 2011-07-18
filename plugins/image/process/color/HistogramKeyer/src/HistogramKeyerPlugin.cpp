#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerProcess.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/algorithm.hpp>
#include <boost/smart_ptr/scoped_array.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

/*
 * fecth plugin IHM parameters with plugin parameters pointer
 */
HistogramKeyerPlugin::HistogramKeyerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramGlobalDisplaySelection = fetchBooleanParam(kGlobalDisplay);			//global display
	
	_paramColorRGBSelection = fetchParametricParam( kParamRGBColorSelection );	//curve RGB
	_paramOverlayRSelection = fetchBooleanParam( kBoolRed );					//R (is channel selected?)
	_paramOverlayGSelection = fetchBooleanParam( kBoolGreen );					//G (is channel selected?)
	_paramOverlayBSelection = fetchBooleanParam( kBoolBlue );					//B (is channel selected?)
	_clearRGB = fetchPushButtonParam(kButtonCleanRGB);							//clean RGB button
	
	_paramColorHSLSelection = fetchParametricParam( kParamHSLColorSelection );	//curve HSL
	_paramOverlayHSelection = fetchBooleanParam( kBoolHue );					//H (is channel selected?)
	_paramOverlaySSelection = fetchBooleanParam( kBoolSaturation );				//S (is channel selected?)
	_paramOverlayLSelection = fetchBooleanParam( kBoolLightness);				//L (is channel selected?)
	_clearHSL = fetchPushButtonParam(kButtonCleanHSL);							//clean HSL button
	
	_paramDisplayTypeSelection = fetchChoiceParam(kHistoDisplayListParamLabel);	//histogram display list (Histogram overlay group)
	_paramClearAll = fetchPushButtonParam(kButtonCleanAll);							//clean all button (Histogram overlay group)

	_paramDisplaySelection = fetchBooleanParam( kBoolSelection);				//display selection on source clip (Selection group)
	
	_paramNbStepSelection = fetchIntParam(knbStepRange);								//nb step range (Advanced group)
	_paramSelectionMultiplierSelection = fetchDoubleParam(kselectionMultiplier);		//selection multiplier (Advanced group)
	_paramRefreshOverlaySelection = fetchPushButtonParam(kButtonRefreshOverlay);		//refresh overlay (Advanced group)
	
	_paramOutputSettingSelection = fetchChoiceParam( kOutputListParamLabel );	//output type (BW/alpha)
	_paramReverseMaskSelection = fetchBooleanParam( kBoolReverseMask);			//reverse mask

	//Reset param booleans
	_isCleaned = false;
	_isNbStepChanged = false;
	_isHistogramRefresh = false;
	
	//Initialize scoped pointer
	_overlayDataCount = 0;
	
	/// @todo: HACK to display curves default position
	OFX::InstanceChangedArgs changed( this->timeLineGetTime() );
	changedParam(changed,kButtonCleanAll); 
	changedClip( changed, kOfxImageEffectSimpleSourceClipName );
}

/*
 * create and return HistogramKeyerProcessParams structure for process
 */
HistogramKeyerProcessParams<HistogramKeyerPlugin::Scalar> HistogramKeyerPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	HistogramKeyerProcessParams<Scalar> params;
	
	params._paramColorRGB = _paramColorRGBSelection;			//curve RGB
	params._boolRGB.assign(3,NULL);
	params._boolRGB.at(0) = _paramOverlayRSelection;			//R (is channel selected?)
	params._boolRGB.at(1) = _paramOverlayGSelection;			//G (is channel selected?)
	params._boolRGB.at(2) = _paramOverlayBSelection;			//B (is channel selected?)
	
	params._paramColorHSL = _paramColorHSLSelection;			//curve HSL
	params._boolHSL.assign(3,NULL);
	params._boolHSL.at(0) = _paramOverlayHSelection;			//H (is channel selected?)
	params._boolHSL.at(1) = _paramOverlaySSelection;			//S (is channel selected?)
	params._boolHSL.at(2) = _paramOverlayLSelection;			//L (is channel selected?)
		
	params._paramOutputSetting = _paramOutputSettingSelection;	//output selection (alpha channel or BW)
	params._boolReverseMask = _paramReverseMaskSelection;		//reverse mask check box
	
	return params;
}

/*
 * a plugin parameter as been changed
 */
void HistogramKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	/*Clean buttons*/
	if( paramName == kButtonCleanRGB || paramName == kButtonCleanHSL || paramName == kButtonCleanAll ) //HSL or RGB or both
	{
		//get nb points for each curve (RGB)
		std::vector<int> nbControlPointsRGB;					//initialize vector
		nbControlPointsRGB.assign(nbCurvesRGB,0);				//assign vector
		for(unsigned int i=0; i< nbCurvesRGB; ++i)				//fill up vector
			nbControlPointsRGB.at(i)= _paramColorRGBSelection->getNControlPoints(i,args.time);
		
		//get nb points for each curve (HSL)
		std::vector<int> nbControlPointsHSL;					//initialize vector
		nbControlPointsHSL.assign(nbCurvesHSL,0);				//assign vector
		for(unsigned int i=0; i< nbCurvesHSL; ++i)				//fill up vector
			nbControlPointsHSL.at(i)= _paramColorHSLSelection->getNControlPoints(i,args.time);
		
		//reset RGB curves
		if(paramName == kButtonCleanRGB || paramName == kButtonCleanAll)//RGB or Clean all
		{
			for(unsigned int channel=0; channel<nbCurvesRGB; ++channel)
			{
				for(unsigned int i=0; i<nbControlPointsRGB[channel]; ++i)
					_paramColorRGBSelection->deleteControlPoint(i);
			}
			for(int i=0; i<nbCurvesRGB; ++i)//replace default points							
			{	
				_paramColorRGBSelection->addControlPoint(i,args.time,0.0,0.0,false);
				_paramColorRGBSelection->addControlPoint(i,args.time,0.2,1.0,false);
				_paramColorRGBSelection->addControlPoint(i,args.time,0.6,1.0,false);
				_paramColorRGBSelection->addControlPoint(i,args.time,1.0,0.0,false);
			}
		}
		//reset HSL curves
		if(paramName == kButtonCleanHSL || paramName == kButtonCleanAll)//HSL or Clean all
		{
			for(unsigned int channel=0; channel<nbCurvesHSL; ++channel)
			{
				for(unsigned int i=0; i<nbControlPointsHSL[channel]; ++i)
					_paramColorHSLSelection->deleteControlPoint(i);
			}
			for(int i=0; i<nbCurvesHSL; ++i)//replace default points
			{	
				_paramColorHSLSelection->addControlPoint(i,args.time,0.0,0.0,false);
				_paramColorHSLSelection->addControlPoint(i,args.time,0.2,1.0,false);
				_paramColorHSLSelection->addControlPoint(i,args.time,0.6,1.0,false);
				_paramColorHSLSelection->addControlPoint(i,args.time,1.0,0.0,false);
			}
		}
		/// @todo How to request a redraw on ParametricParameters?
		//	_paramColorHSLSelection->getProps().propGetPointer( kOfxParamPropParametricInteractBackground );
	}
	
	/*refresh histogram overlay*/
	else if( paramName == kButtonRefreshOverlay )
	{
		//Draw forced
		OFX::InstanceChangedArgs changed( args.time, args.renderScale );
		this->changedClip(changed,this->_clipSrc->name());
	}
	/*nbStep changed*/
	else if( paramName == knbStepRange )
	{
		if( this->hasOverlayData() ) //if there is overlay value
		{
			getOverlayData().setNbStep( _paramNbStepSelection->getValue() ); //change nbStep value
			getOverlayData().computeFullData( this->_clipSrc, args.time, args.renderScale ); //reset buffer and compute them
		}
	}
	/*Clear user selection*/
	else if( paramName == kButtonClearSelection )
	{
		if(this->hasOverlayData())//if there is overlay value
		{
			this->getOverlayData().clearSelection();//clear selection
		}
	}
}

/*
 * the source clip has been changed
 */
void HistogramKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		if( this->hasOverlayData() )
		{
			this->getOverlayData().clearAll( this->_clipSrc->getPixelRodSize( args.time, args.renderScale ) );
			this->getOverlayData().computeFullData( this->_clipSrc, args.time, args.renderScale );
		}
	}
}

/*
 * does plugin do something
 */
bool HistogramKeyerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	/// @todo HACK: nuke doesn't call changedClip when the time is modified.
	//OFX::InstanceChangedArgs changed( args.time, args.renderScale );
	//this->changedClip(changed,this->_clipSrc->name());
	//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void HistogramKeyerPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<HistogramKeyerProcess>( *this, args );
}

/// @brief Overlay data
/// @{
/*Overlay data management*/
void HistogramKeyerPlugin::addRefOverlayData()
{
	if( _overlayDataCount == 0 )
	{
		const OfxPointI imgSize = this->_clipSrc->getPixelRodSize(0); ///@todo set the correct time !
		_overlayData.reset( new OverlayData( imgSize, this->_paramNbStepSelection->getValue() ) );
	}
	++_overlayDataCount;
}
void HistogramKeyerPlugin::releaseOverlayData()
{
	--_overlayDataCount;
	if(_overlayDataCount == 0)
	{
		_overlayData.reset(NULL);
	}
}
bool HistogramKeyerPlugin::hasOverlayData() const
{
	return _overlayDataCount != 0;
}
OverlayData& HistogramKeyerPlugin::getOverlayData()
{
	return *_overlayData.get();
}
const OverlayData& HistogramKeyerPlugin::getOverlayData() const
{
	return *_overlayData.get();
}
/// @}
}
}
}
