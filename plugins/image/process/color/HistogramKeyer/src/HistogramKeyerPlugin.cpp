#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerProcess.hpp"
#include "HistogramKeyerDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/algorithm.hpp>

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
	_clearAll = fetchPushButtonParam(kButtonCleanAll);							//clean all button (Histogram overlay group)

	_paramDisplaySelection = fetchBooleanParam( kBoolSelection);				//display selection on source clip (Selection group)
	
	_nbStepSelection = fetchIntParam(knbStepRange);								//nb step range (Advanced group)
	_selectionMultiplierSelection = fetchDoubleParam(kselectionMultiplier);		//selection multiplier (Advanced group)
	_refreshOverlaySelection = fetchPushButtonParam(kButtonRefreshOverlay);		//refresh overlay (Advanced group)
	
	_paramOutputSettingSelection = fetchChoiceParam( kOutputListParamLabel );	//output type (BW/alpha)
	_paramReverseMaskSelection = fetchBooleanParam( kBoolReverseMask);			//reverse mask

	//Reset Histogram buffers
	this->_data._step = nbStep;
	this->resetHistogramBufferData(this->_data);
	//Reset Histogram selection buffers
	this->_selectionData._step = nbStep;
	this->resetHistogramBufferData(this->_selectionData);
	//Reset param booleans
	_isCleaned = false;
	_isNbStepChanged = false;
	
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
	if(paramName == kButtonCleanRGB || paramName == kButtonCleanHSL || paramName == kButtonCleanAll) //HSL or RGB or both
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
	if(paramName == kButtonRefreshOverlay)
	{
		//Draw forced
		OFX::InstanceChangedArgs changed( args.time, args.renderScale );
		this->changedClip(changed,this->_clipSrc->name());
	}
	/*nbStep changed*/
	if(paramName == knbStepRange)
	{
		nbStep = _nbStepSelection->getValue(); //get nb step value
		OFX::InstanceChangedArgs changed( args.time, args.renderScale );	
		this->changedClip(changed,this->_clipSrc->name()); // recompute histograms data
		this->_isNbStepChanged = true;	//reload histograms in overlay
	}
	/*Clear user selection*/
	if(paramName == kButtonClearSelection)
	{
		_isCleaned = true; // clear selection display
		this->resetHistogramBufferData(_selectionData); //reset selection data buffers
	}

}

/*
 * the source clip has been changed
 */
void HistogramKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		/*Compatibility tests */
		boost::scoped_ptr<OFX::Image> src( this->_clipSrc->fetchImage( args.time ) ); //scoped pointer of current source clip
		if( !src.get() )//is source is not available
		{
			this->resetHistogramBufferData(this->_data);// reset histogram buffers
			return;
		}
		if( src->getRowBytes() == 0 )// if source is wrong
		{
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		}
				
		OfxRectI srcPixelRod = this->_clipSrc->getPixelRod( args.time, args.renderScale );	//get current RoD
		if((this->_clipDst->getPixelDepth() != OFX::eBitDepthFloat)||(!this->_clipSrc->getPixelComponents())) 
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Can't compute histogram data with the actual input clip format." );
                        return;
		}
		/*Compute if source is OK*/
		//Get view
		typedef boost::gil::rgba32f_view_t SView;								//declare current view type
		SView srcView = tuttle::plugin::getView<SView>(src.get(), srcPixelRod);	//get current view from source clip
		//Prepare histograms
		this->_data._step = nbStep;												//prepare HistogramBuffer structure
		this->resetHistogramBufferData(this->_data);							//set HistogramBuffer structure to null
		
		//create HistogramData with a functor
		Pixel_increment_histogramData funct;									
		funct._data._step = this->_data._step;									//prepare functor HistogramBuffer structure
		this->resetHistogramBufferData(funct._data);							//set functor HistogramBuffer structure to null
		
		boost::gil::transform_pixels( srcView, funct ); //(USED functor reference)
		//boost::gil::for_each_pixel(srcView, funct);	  (NOT USED)
		
		this->_data = funct._data ;												//translate functor ephemeral data to real data
		this->correctHistogramBufferData(this->_data);							//correct Histogram data to make up for discretization (average)
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

/**
 * @brief Set each values of the vector to null
 * @param v vector to reset
 * @param numberOfStep number of step (size of the vector)
 */
void HistogramKeyerPlugin::resetVectortoZero( std::vector<Number>& v, const unsigned int numberOfStep ) const
{
	v.assign(numberOfStep,0);
}

/**
 * @brief Set each values of the vector to null
 * @param toReset HistogramBufferdata instance to reset
 */
void HistogramKeyerPlugin::resetHistogramBufferData( HistogramBufferData& toReset ) const
{
	//Alpha
	this->resetVectortoZero(toReset._bufferAlpha,toReset._step);				//alpha
	//RGB
	this->resetVectortoZero(toReset._bufferRed,toReset._step);					//R
	this->resetVectortoZero(toReset._bufferGreen,toReset._step);				//G
	this->resetVectortoZero(toReset._bufferBlue,toReset._step);					//B
	//HLS
	this->resetVectortoZero(toReset._bufferHue,toReset._step);					//H
	this->resetVectortoZero(toReset._bufferLightness,toReset._step);			//S
	this->resetVectortoZero(toReset._bufferSaturation,toReset._step);			//L
}

/**
 * Correct the HistogramBufferData buffers wrong value with an average
 * @param toCorrect HistogramBufferData to correct
 */
void HistogramKeyerPlugin::correctHistogramBufferData(HistogramBufferData& toCorrect) const
{
	//RGB
	this->correctVector(toCorrect._bufferRed);									//R
	this->correctVector(toCorrect._bufferGreen);								//G
	this->correctVector(toCorrect._bufferBlue);									//B
	//HSL
	this->correctVector(toCorrect._bufferHue);									//H
	this->correctVector(toCorrect._bufferSaturation);							//S
	this->correctVector(toCorrect._bufferLightness);							//L
	
}

/**
 * Replace vector null values by average (better for histogram display) 
 * @param v vector to modify
 */
void HistogramKeyerPlugin::correctVector(std::vector<Number>& v) const
{
	for(unsigned int i=1; i<v.size()-1;++i)
	{
		if(v.at(i) < 0.05)
			v.at(i) = (Number)((v.at(i-1)+v.at(i+1))/2.0);//basic average
	}
}

}
}
}
