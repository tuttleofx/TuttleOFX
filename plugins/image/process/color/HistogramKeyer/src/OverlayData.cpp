#include "OverlayData.hpp"

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	
/**
 * Create a new empty data structure from scratch (data is null)
 * @param size : size of the current source clip (width*height) 
 */
OverlayData::OverlayData(OfxPointI size) 
{
	_size =  size;
	//allocate and initialize bool img tab 2D
	bool_2d::extent_gen extents;
	_imgBool.resize(extents[_size.y][_size.x]);	
	for(unsigned int i=0; i<_size.y; ++i)
	{
		for(unsigned int j=0; j<_size.x; ++j)
			_imgBool[i][j] = 0;
	}
	vNbStep = 255;
	//Reset Histogram buffers
	this->_data._step = vNbStep;
	this->resetHistogramBufferData(this->_data);
	//Reset Histogram selection buffers
	this->_selectionData._step = vNbStep;
	this->resetHistogramBufferData(this->_selectionData);
}

/**
 * Update selection areas buffer to selection histograms overlay
 * @param args needed to have current time
 */
void OverlayData::computeHistogramBufferData(HistogramBufferData& data,OFX::Clip* clipSrc,const OfxTime time, const OfxPointD renderScale,bool isSelection)
{
	//TUTTLE_COUT_INFOS;
	//TUTTLE_COUT_VAR( "computeHistogramBufferData - fetchImage " << time );
	boost::scoped_ptr<OFX::Image> src( clipSrc->fetchImage(time, clipSrc->getCanonicalRod(time)) );	//scoped pointer of current source clip
	//TUTTLE_COUT_INFOS;
	//TUTTLE_COUT_VAR( clipSrc->getCanonicalRod(time) );
	//TUTTLE_COUT_VAR( src->getBounds() );
	
	/*Compatibility tests */
	if( !src.get() ) // source isn't accessible
	{
		return;
	}
	if( src->getRowBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	}
	OfxRectI srcPixelRod = clipSrc->getPixelRod( time,renderScale ); //get current RoD
	if((clipSrc->getPixelDepth() != OFX::eBitDepthFloat)||(!clipSrc->getPixelComponents())) 
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return;
	}
	
	/*Compute if source is OK*/
	typedef boost::gil::rgba32f_view_t SView;								//declare current view type
	SView srcView = tuttle::plugin::getView<SView>(src.get(),srcPixelRod);	//get current view from source clip
	
	data._step = vNbStep;					//prepare HistogramBuffer structure
	this->resetHistogramBufferData(data);	//set HistogramBuffer structure to null	
	
	Pixel_compute_histograms funct( _imgBool );			//functor declaration
	funct._width = _size.x;					//width
	funct._height= _size.y;					//height
	funct._y = 0;							//initialize current pixel x to 0
	funct._x = 0;							//initialize current pixel y to 0
	funct._data = data;						//copy data
	funct._isSelectionMode = isSelection;	//do we work on selection or normal histograms
	
	//TUTTLE_COUT_INFOS;
	
	boost::gil::transform_pixels( srcView, funct );		 //(USED functor reference)
	//boost::gil::for_each_pixel(srcView, funct);		(NOT USED)
	
	//TUTTLE_COUT_INFOS;
	data = funct._data ;								//translate functor ephemeral data to real data
	this->correctHistogramBufferData(data);				//correct Histogram data to make up for discretization (average)
	//TUTTLE_COUT_INFOS;
}

/**
 * @brief Set each values of the vector to null
 * @param v vector to reset
 * @param numberOfStep number of step (size of the vector)
 */
void OverlayData::resetVectortoZero( std::vector<Number>& v, const unsigned int numberOfStep ) const
{
	v.assign(numberOfStep,0);
}

/**
 * @brief Set each values of the vector to null
 * @param toReset HistogramBufferdata instance to reset
 */
void OverlayData::resetHistogramBufferData( HistogramBufferData& toReset ) const
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
void OverlayData::correctHistogramBufferData(HistogramBufferData& toCorrect) const
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
void OverlayData::correctVector(std::vector<Number>& v) const
{
	for(unsigned int i=1; i<v.size()-1;++i)
	{
		if(v.at(i) < 0.05)
			v.at(i) = (Number)((v.at(i-1)+v.at(i+1))/2.0);//basic average
	}
}

/**
 * Compute average bars for display
 */
void OverlayData::computeAverages()
{
	//RGB
	this->_averageData._averageRed = computeAnAverage(this->_selectionData._bufferRed);				//R
	this->_averageData._averageBlue = computeAnAverage(this->_selectionData._bufferBlue);			//G
	this->_averageData._averageGreen = computeAnAverage(this->_selectionData._bufferGreen);			//B
	//HSL
	this->_averageData._averageHue = computeAnAverage(this->_selectionData._bufferHue);					//H
	this->_averageData._averageSaturation = computeAnAverage(this->_selectionData._bufferSaturation);	//S
	this->_averageData._averageLightness = computeAnAverage(this->_selectionData._bufferLightness);		//L
}

/**
 * Compute a specific channel average
 * @param selection_v vector which contain the selection histogram
 * @return 
 */
int OverlayData::computeAnAverage(std::vector<Number> selection_v) const
{
	int av = 0;
	int size = 0;
	for(unsigned int i=0; i<selection_v.size(); ++i)
	{
		if(selection_v.at(i)!=0)
		{
			av+=selection_v.at(i)*i;
			size+=selection_v.at(i);
		}
	}
	if(size != 0) //avoid 0 division
		av /=size;
	return av; //basic average
}

/**
 * Compute full data (averages,histograms buffer and selection buffer)
 * @param clipSrc	source of the plugin
 * @param time	current time
 * @param renderScale	current renderScale
 */
void OverlayData::computeFullData(OFX::Clip* clipSrc,const OfxTime time, const OfxPointD renderScale)
{
	//Reset Histogram buffers
	this->_data._step = vNbStep;
	this->resetHistogramBufferData(this->_data);
	//Reset Histogram selection buffers
	this->_selectionData._step = vNbStep;
	this->resetHistogramBufferData(this->_selectionData);
	
	
	//Compute histogram buffer
	this->computeHistogramBufferData(_data,clipSrc,time,renderScale);
	this->correctHistogramBufferData(_data);
	
	//Compute selection histogram buffer
	this->computeHistogramBufferData(_selectionData,clipSrc,time,renderScale,true);
	this->correctHistogramBufferData(_selectionData);
	//Compute averages
	this->computeAverages();
	
	Number maxR = *(std::max_element(_data._bufferRed.begin(),_data._bufferRed.end()));
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetData(OfxPointI size)
{
	//change size
	_size =  size;
	//allocate and initialize bool img tab 2D
	bool_2d::extent_gen extents;
	_imgBool.resize(extents[_size.y][_size.x]);	
	for(unsigned int i=0; i<_size.y; ++i)
	{
		for(unsigned int j=0; j<_size.x; ++j)
			_imgBool[i][j] = 0;
	}
	
	//Reset Histogram buffers
	this->_data._step = vNbStep;
	this->resetHistogramBufferData(this->_data);
	//Reset Histogram selection buffers
	this->_selectionData._step = vNbStep;
	this->resetHistogramBufferData(this->_selectionData);
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetSelectionData(OfxPointI size)
{
	//change size
	_size =  size;
	//allocate and initialize bool img tab 2D
	bool_2d::extent_gen extents;
	_imgBool.resize(extents[_size.y][_size.x]);	
	for(unsigned int i=0; i<_size.y; ++i)
	{
		for(unsigned int j=0; j<_size.x; ++j)
			_imgBool[i][j] = 0;
	}
	//Reset Histogram selection buffers
	this->_selectionData._step = vNbStep;
	this->resetHistogramBufferData(this->_selectionData);
}

/**
 * Set all of averages to 0
 */
void OverlayData::resetAverages()
{
	//reset average
	this->_averageData._averageRed = 0;			//R
	this->_averageData._averageGreen = 0;		//G
	this->_averageData._averageBlue = 0;		//B
	this->_averageData._averageHue = 0;			//H
	this->_averageData._averageSaturation = 0;	//S
	this->_averageData._averageLightness = 0;	//L
}

}
}
}