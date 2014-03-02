#include "OverlayData.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <terry/algorithm/transform_pixels.hpp>

namespace tuttle {
namespace plugin {
namespace diffPlot {

/**
 * Create a new empty data structure from scratch (data is null)
 * @param size : size of the current source clip (width*height) 
 */
OverlayData::OverlayData( const OfxPointI& size, const int nbSteps )
: _currentTime( 0 )
, _vNbStep( nbSteps )
, _isComputing( false )
, _isDataInvalid( true )
, _size( size )
{
}

/**
 * Update selection areas buffer to selection diffPlots overlay
 * @param args needed to have current time
 */
void OverlayData::computeDiffPlotBufferData( DiffPlotBufferData& data, SView& srcAView, SView& srcBView, const OfxTime time, const bool isSelection)
{
	data._step = _vNbStep;					//prepare DiffPlotBuffer structure
	
	BOOST_ASSERT( _imgBool.shape()[0] == std::size_t(_size.y) );
	BOOST_ASSERT( _imgBool.shape()[1] == std::size_t(_size.x) );
	BOOST_ASSERT( srcAView.width()  == std::size_t(_size.x) );
	BOOST_ASSERT( srcAView.height() == std::size_t(_size.y) );
	BOOST_ASSERT( srcBView.width()  == std::size_t(_size.x) );
	BOOST_ASSERT( srcBView.height() == std::size_t(_size.y) );

	Pixel_compute_diffPlots funct( _imgBool, data, isSelection );
	terry::algorithm::transform_pixels( srcAView, srcBView, funct );
}

/**
 * @brief Set each values of the vector to null
 * @param toReset DiffPlotBufferdata instance to reset
 */
void OverlayData::resetDiffPlotBufferData( DiffPlotBufferData& toReset ) const
{
	//RGB
	toReset._bufferRed.clear();
	toReset._bufferGreen.clear();
	toReset._bufferBlue.clear();
	//HLS
	toReset._bufferHue.clear();
	toReset._bufferLightness.clear();
	toReset._bufferSaturation.clear();
}

/**
 * Compute full data (averages,diffPlots buffer and selection buffer)
 * @param clipSrcA  source of the plugin
 * @param clipSrcB  source of the plugin
 * @param time  current time
 * @param renderScale  current renderScale
 */
void OverlayData::computeFullData( OFX::Clip* clipSrcA, OFX::Clip* clipSrcB, const OfxTime time, const OfxPointD& renderScale, const bool selectionOnly )
{
	_isComputing = true;
	resetDiffPlotData();
	resetDiffPlotSelectionData();

	if( ! clipSrcA->isConnected() || ! clipSrcB->isConnected() )
	{	
		_isComputing = false;
		return;
	}

	boost::scoped_ptr<OFX::Image> srcA( clipSrcA->fetchImage( time ) );
	if( ! srcA.get() )
	{	
		_isComputing = false;
		return;
	}
	if( srcA->getRowDistanceBytes() == 0 )
	{	
		_isComputing = false;
		return;
	}
	// Compatibility tests
	if( (clipSrcA->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipSrcA->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute diffPlot data with the actual input clip format." );
		return;
	}
	OfxRectI srcAPixelRod = clipSrcA->getPixelRod( time, renderScale );  // get current RoD
	if( srcAPixelRod != srcA->getBounds() )
	{
		// the host does bad things !
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcAPixelRod << " , bounds:" << srcA->getBounds() << ")." );
		return;
	}

	boost::scoped_ptr<OFX::Image> srcB( clipSrcB->fetchImage( time ) );
	if( ! srcB.get() )
	{	
		_isComputing = false;
		return;
	}
	if( srcB->getRowDistanceBytes() == 0 )
	{	
		_isComputing = false;
		return;
	}
	// Compatibility tests
	if( (clipSrcB->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipSrcB->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute diffPlot data with the actual input clip format." );
		return;
	}
	OfxRectI srcBPixelRod = clipSrcB->getPixelRod( time, renderScale );  // get current RoD
	if( srcBPixelRod != srcB->getBounds() )
	{
		// the host does bad things !
		// remove overlay... but do not crash.
		TUTTLE_LOG_WARNING( "Image RoD and image bounds are not the same (rod=" << srcBPixelRod << " , bounds:" << srcB->getBounds() << ")." );
		return;
	}
	if( srcAPixelRod != srcBPixelRod )
	{
		// Not supported yet... should use the intersection...
		TUTTLE_LOG_WARNING( "Input A and input B doesn't have the same RoD (A rod=" << srcAPixelRod << ", B rod=" << srcBPixelRod << ")." );
		return;
	}

	// Compute if source is OK
	SView srcAView = tuttle::plugin::getGilView<SView>( srcA.get(), srcAPixelRod, eImageOrientationIndependant );	// get current view from source clip
	SView srcBView = tuttle::plugin::getGilView<SView>( srcB.get(), srcBPixelRod, eImageOrientationIndependant );	// get current view from source clip

	// Compute diffPlot buffer
	this->computeDiffPlotBufferData( _data, srcAView, srcBView, time, false );

//	 // Compute selection diffPlot buffer
//	this->computeDiffPlotBufferData( _selectionData, srcView, time, true );

	_isComputing = false;
	_currentTime = time;
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetDiffPlotData()
{
	// Reset DiffPlot buffers
	this->_data._step = _vNbStep;
	this->resetDiffPlotBufferData(this->_data);
}

/**
 * Reset the data (all values to 0)
 * @param size size of the current source clip
 */
void OverlayData::resetDiffPlotSelectionData()
{
	//Reset DiffPlot selection buffers
	this->_selectionData._step = _vNbStep;
	this->resetDiffPlotBufferData(this->_selectionData);
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

}
}
}
