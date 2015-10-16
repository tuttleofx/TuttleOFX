#include "OverlayData.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <terry/algorithm/transform_pixels.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

/**
 * Create a new empty data structure from scratch (data is null)
 * @param size : size of the current source clip (width*height) 
 */
OverlayData::OverlayData( const OfxPointI& size, const int nbSteps, const int nbStepsCurvesFromSelection)
: _currentTime( 0 )
, _vNbStep( nbSteps )
, _vNbStepCurveFromSelection( nbStepsCurvesFromSelection )
, _isComputing( false )
, _isDataInvalid( true )
, _size( size )
{
	clearAll( size );
}

/**
 * Update selection areas buffer to selection histograms overlay
 * @param args needed to have current time
 */
void OverlayData::computeHistogramBufferData( HistogramBufferData& data, SView& srcView, const OfxTime time, const bool isSelection)
{
	data._step = _vNbStep;					//prepare HistogramBuffer structure
	
	BOOST_ASSERT( _imgBool.shape()[0] == std::size_t(_size.y) );
	BOOST_ASSERT( _imgBool.shape()[1] == std::size_t(_size.x) );
	BOOST_ASSERT( srcView.width()  == std::size_t(_size.x) );
	BOOST_ASSERT( srcView.height() == std::size_t(_size.y) );
	
	Pixel_compute_histograms funct( _imgBool, data, isSelection );			//functor declaration
	
	terry::algorithm::transform_pixels( srcView, funct );		//(USED functor reference)
	//boost::gil::for_each_pixel(srcView, funct);		(NOT USED)
	
	this->correctHistogramBufferData(data);				//correct Histogram data to make up for discretization (average)
}

/**
 * @brief Set each values of the vector to null
 * @param v vector to reset
 * @param numberOfStep number of step (size of the vector)
 */
void OverlayData::resetVectortoZero( HistogramVector& v, const std::size_t numberOfStep ) const
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
void OverlayData::correctVector( HistogramVector& v ) const
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
int OverlayData::computeAnAverage( const HistogramVector& selection_v ) const
{
	int av = 0;
	int size = 0;
	for( std::size_t i=0; i < selection_v.size(); ++i)
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
void OverlayData::computeFullData( OFX::Clip* clipSrc, const OfxTime time, const OfxPointD& renderScale, const bool selectionOnly )
{
	_isComputing = true;
	resetHistogramData();
	resetHistogramSelectionData();
	
	if( ! clipSrc->isConnected() )
	{	
		_isComputing = false;
		return;
	}
	
	//TUTTLE_LOG_INFOS;
	//TUTTLE_LOG_VAR( TUTTLE_INFO, "computeHistogramBufferData - fetchImage " << time );
	boost::scoped_ptr<OFX::Image> src( clipSrc->fetchImage(time, clipSrc->getCanonicalRod(time, renderScale)) );	//scoped pointer of current source clip
	//TUTTLE_LOG_INFOS;
	
	//TUTTLE_LOG_VAR( TUTTLE_INFO, clipSrc->getPixelRod(time, renderScale) );
	//TUTTLE_LOG_VAR( TUTTLE_INFO, clipSrc->getCanonicalRod(time, renderScale) );
	
	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		_isComputing = false;
		std::cout << "src is not accessible" << std::endl;
		return;
	}
	
//	TUTTLE_LOG_VAR( TUTTLE_INFO, src->getBounds() );
//	TUTTLE_LOG_VAR( TUTTLE_INFO, src->getRegionOfDefinition() );

	if( src->getRowDistanceBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	}
	OfxRectI srcPixelRod = clipSrc->getPixelRod( time, renderScale ); //get current RoD
	if( (clipSrc->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipSrc->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return;
	}
	
//	TUTTLE_LOG_INFOS;
//	BOOST_ASSERT( srcPixelRod == src->getBounds() );
	if( srcPixelRod != src->getBounds() )
	{
		// the host does bad things !
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return;
	}
//	BOOST_ASSERT( srcPixelRod.x1 == src->getBounds().x1 );
//	BOOST_ASSERT( srcPixelRod.y1 == src->getBounds().y1 );
//	BOOST_ASSERT( srcPixelRod.x2 == src->getBounds().x2 );
//	BOOST_ASSERT( srcPixelRod.y2 == src->getBounds().y2 );
	
	// Compute if source is OK
	SView srcView = tuttle::plugin::getGilView<SView>( src.get(), srcPixelRod, eImageOrientationIndependant );	// get current view from source clip
	
	OfxPointI imgSize;
	imgSize.x = srcView.width();
	imgSize.y = srcView.height();
	
//	TUTTLE_LOG_INFOS;
	if( isImageSizeModified( imgSize ) )
	{
		//TUTTLE_LOG_INFOS;
		clearAll( imgSize );
	}
	
	//TUTTLE_LOG_INFOS;
	//Compute histogram buffer
	this->computeHistogramBufferData( _data, srcView, time);
	
	//TUTTLE_LOG_INFOS;
	//Compute selection histogram buffer
	this->computeHistogramBufferData( _selectionData, srcView, time, true );
	
	//TUTTLE_LOG_INFOS;
	//Compute averages
	this->computeAverages();
	_isComputing = false;
	
	_currentTime = time;
	//TUTTLE_LOG_INFOS;
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetHistogramData()
{
	// Reset Histogram buffers
	this->_data._step = _vNbStep;
	this->resetHistogramBufferData(this->_data);
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetCurvesFromSelectionData()
{
	// Reset Histogram buffers
	this->_curveFromSelection._step = _vNbStepCurveFromSelection;
	this->resetHistogramBufferData(this->_curveFromSelection);
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetHistogramSelectionData()
{
	//Reset Histogram selection buffers
	this->_selectionData._step = _vNbStep;
	this->resetHistogramBufferData(this->_selectionData);
}

void OverlayData::removeSelection()
{
	//allocate and initialize bool img tab 2D
	bool_2d::extent_gen extents;
	_imgBool.resize(extents[_size.y][_size.x]);

	for( unsigned int i=0; i < _imgBool.shape()[0]; ++i )
	{
		for( unsigned int j=0; j < _imgBool.shape()[1]; ++j )
		{
			_imgBool[i][j] = 0;
		}
	}
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

/**
 * Check size (verify that imgBool always has the good size
 */
bool OverlayData::isImageSizeModified( const OfxPointI& imgSize ) const
{	
	return( _size.x != imgSize.x ||
		_size.y != imgSize.y );
}

/**
 * Current time checker
 */
bool OverlayData::isCurrentTimeModified(const OfxTime time) const
{
		return( _currentTime != time );
}	

/**
 * Compute only curve from selection data (averages,histograms buffer and selection buffer)
 * @param clipSrc	source of the plugin
 * @param time	current time
 * @param renderScale	current renderScale
 */
void OverlayData::computeCurveFromSelectionData( OFX::Clip* clipSrc, const OfxTime time, const OfxPointD& renderScale)
{
	_isComputing = true;

	resetCurvesFromSelectionData();
	
	if( ! clipSrc->isConnected() )
	{	
		_isComputing = false;
		return;
	}
	boost::scoped_ptr<OFX::Image> src( clipSrc->fetchImage(_currentTime, clipSrc->getCanonicalRod(_currentTime, renderScale)) );	//scoped pointer of current source clip

	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		_isComputing = false;
		std::cout << "src is not accessible" << std::endl;
		return;
	}

	if( src->getRowDistanceBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	}
	OfxRectI srcPixelRod = clipSrc->getPixelRod( _currentTime, renderScale ); //get current RoD
	if( (clipSrc->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipSrc->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
		return;
	}

	if( srcPixelRod != src->getBounds() )
	{
		// the host does bad things !
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return;
	}
	
	// Compute if source is OK
	SView srcView = tuttle::plugin::getGilView<SView>( src.get(), srcPixelRod, eImageOrientationIndependant );	// get current view from source clip
	
	OfxPointI imgSize;
	imgSize.x = srcView.width();
	imgSize.y = srcView.height();
	
	if( isImageSizeModified( imgSize ) )
	{
		clearAll( imgSize );
	}
	//Compute histogram buffer
	Pixel_compute_histograms funct( _imgBool,_curveFromSelection, true);			//functor declaration
	terry::algorithm::transform_pixels( srcView, funct );		//(USED functor reference)
	
	this->correctHistogramBufferData(_curveFromSelection);				//correct Histogram data to make up for discretization (average)
}

}
}
}
