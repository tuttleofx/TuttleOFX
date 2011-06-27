#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerProcess.hpp"
#include "HistogramKeyerDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/algorithm.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {


HistogramKeyerPlugin::HistogramKeyerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	//Curves
	_paramColorRGBSelection = fetchParametricParam( kParamRGBColorSelection );
	_paramColorHSLSelection = fetchParametricParam( kParamHSLColorSelection );
	//Overlay height option
	_paramDisplayTypeSelection = fetchChoiceParam( kHistoDisplayListParamLabel );
	//Bool
	_paramOverlayRSelection = fetchBooleanParam( kBoolRed );
	_paramOverlayGSelection = fetchBooleanParam( kBoolGreen );
	_paramOverlayBSelection = fetchBooleanParam( kBoolBlue );
	_paramOverlayHSelection = fetchBooleanParam( kBoolHue );
	_paramOverlaySSelection = fetchBooleanParam( kBoolSaturation );
	_paramOverlayLSelection = fetchBooleanParam( kBoolLightness);
	//Groups
	_groupRGBSelection = fetchGroupParam(kGroupRGB);
	_groupHSLSelection = fetchGroupParam(kGroupHSL);
	//Push buttons
	_clearRGB = fetchPushButtonParam(kButtonCleanRGB);
	_clearHSL = fetchPushButtonParam(kButtonCleanHSL);
	_clearAll = fetchPushButtonParam(kButtonCleanAll);
	_refreshOverlaySelection = fetchPushButtonParam(kButtonRefreshOverlay);
	//int range
	_nbStepSelection = fetchIntParam(knbStepRange);

	//Reset buffers
	this->_data._step = nbStep;
	this->resetHistogramBufferData(this->_data);
	OFX::InstanceChangedArgs changed;
	changedClip( changed, kOfxImageEffectSimpleSourceClipName );
}

HistogramKeyerProcessParams<HistogramKeyerPlugin::Scalar> HistogramKeyerPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	HistogramKeyerProcessParams<Scalar> params;
	params._paramColorRGB = _paramColorRGBSelection;
	params._paramColorHSL = _paramColorHSLSelection;
        
//	for( int curveId = 0; curveId < nbCurves; ++curveId )
//	{
//		int n = _paramColorSelection->getNControlPoints( curveId, time );
//		Curve& c = params._curves[curveId];
//		for( int i = 0; i < n; ++i )
//		{
//			std::pair<double, double> v = _paramColorSelection->getNthControlPoints( curveId, time, i );
//			c[v.first] = v.second;
//		}
//	}
	return params;
}

void HistogramKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	//Clean buttons
	if(paramName == kButtonCleanRGB || paramName == kButtonCleanHSL || paramName == kButtonCleanAll)
	{
		//RGB nb points for each curve
		int nbControlPointsRGB[nbCurvesRGB];
		for(unsigned int i=0; i< nbCurvesRGB; ++i)
			nbControlPointsRGB[i]= _paramColorRGBSelection->getNControlPoints(i,args.time);
		//HSL nb points for each curve
		int nbControlPointsHSL[nbCurvesHSL];
		for(unsigned int i=0; i< nbCurvesHSL; ++i)
			nbControlPointsHSL[i]= _paramColorHSLSelection->getNControlPoints(i,args.time);
		//reset RGB curves
		if(paramName == kButtonCleanRGB || paramName == kButtonCleanAll)
		{
			for(unsigned int channel=0; channel<nbCurvesRGB; ++channel)
			{
				for(unsigned int i=0; i<nbControlPointsRGB[channel]; ++i)
				{
					_paramColorRGBSelection->deleteControlPoint(i);
				}
			}
			for(int i=0; i<nbCurvesRGB; ++i)
				_paramColorRGBSelection->addControlPoint(i,args.time,1.0,1.0,false);
		}
		//reset HSL curves
		if(paramName == kButtonCleanHSL || paramName == kButtonCleanAll)
		{
			for(unsigned int channel=0; channel<nbCurvesHSL; ++channel)
			{
				for(unsigned int i=0; i<nbControlPointsHSL[channel]; ++i)
					_paramColorHSLSelection->deleteControlPoint(i);
			}
			for(int i=0; i<nbCurvesHSL; ++i)
				_paramColorHSLSelection->addControlPoint(i,args.time,1.0,1.0,false);
		}
	}
	//refresh histogram overlay
	if(paramName == kButtonRefreshOverlay)
	{
		//Draw forced
		OFX::InstanceChangedArgs changed;
		this->changedClip(changed,this->_clipSrc->name());
	}
	//nbStepRande
	if(paramName == knbStepRange)
	{
		nbStep = _nbStepSelection->getValue();
		OFX::InstanceChangedArgs changed;
		this->changedClip(changed,this->_clipSrc->name());
	}
}

void HistogramKeyerPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		/// @todo robin: recompute histogram datas
		boost::scoped_ptr<OFX::Image> src( this->_clipSrc->fetchImage( args.time ) );
		if( !src.get() )
		{
			this->resetHistogramBufferData(this->_data);
			return;
		}
		if( src->getRowBytes() == 0 )
		{
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		}
		OfxRectI srcPixelRod = this->_clipSrc->getPixelRod( args.time, args.renderScale );
		//compatibility tests
		if((this->_clipDst->getPixelDepth() != OFX::eBitDepthFloat)||(!this->_clipSrc->getPixelComponents())) 
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Can't compute histogram data with the actual input clip format." );
                        return;
		}
		typedef boost::gil::rgba32f_view_t SView;
		SView srcView = tuttle::plugin::getView<SView>( src.get(), srcPixelRod );
		this->_data._step = nbStep;
		this->resetHistogramBufferData(this->_data);
		//create HistogramData with a functor
		Pixel_increment_histogramData funct;
		funct._data._step = this->_data._step;
		this->resetHistogramBufferData(funct._data);

		boost::gil::transform_pixels( srcView, funct ); // (USED functor reference)
		//boost::gil::for_each_pixel(srcView, funct);  (NOT USED)
		this->_data = funct._data ;	
		this->correctHistogramBufferData(this->_data);
	}
}

//void HistogramKeyerPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool HistogramKeyerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
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
	this->resetVectortoZero(toReset._bufferAlpha,toReset._step);
	//RGB
	this->resetVectortoZero(toReset._bufferRed,toReset._step);
	this->resetVectortoZero(toReset._bufferGreen,toReset._step);
	this->resetVectortoZero(toReset._bufferBlue,toReset._step);
	//HLS
	this->resetVectortoZero(toReset._bufferHue,toReset._step);
	this->resetVectortoZero(toReset._bufferLightness,toReset._step);
	this->resetVectortoZero(toReset._bufferSaturation,toReset._step);
}

/**
 * Correct the HistogramBufferData buffers wrong value with an average
 * @param toCorrect HistogramBufferData to correct
 */
void HistogramKeyerPlugin::correctHistogramBufferData(HistogramBufferData& toCorrect) const
{
	//RGB
	this->correctVector(toCorrect._bufferRed);
	this->correctVector(toCorrect._bufferGreen);
	this->correctVector(toCorrect._bufferBlue);
	//HSL
	this->correctVector(toCorrect._bufferHue);
	this->correctVector(toCorrect._bufferSaturation);
	this->correctVector(toCorrect._bufferLightness);
	
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
			v.at(i) = (Number)((v.at(i-1)+v.at(i+1))/2.0);
	}
}
}
}
}
