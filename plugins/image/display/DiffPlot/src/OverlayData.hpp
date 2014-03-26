#ifndef OVERLAYDATA_HPP
#define	OVERLAYDATA_HPP

#include "DiffPlotDefinitions.hpp"
#include "ColorData.hpp"
#include "ChannelData.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace diffPlot {


class OverlayData 
{
public:
	typedef boost::gil::rgba32f_view_t SView; // declare current view type
	
public:
	OverlayData( const OfxPointI& size, const int nbSteps);
	
	/** 
	 * reset selection data (button clear selection)
	 */
	void clearSelection()
	{
		resetDiffPlotSelectionData();
		removeSelection();
	}
	void clearAll( const OfxPointI& size )
	{
		_size = size;
		resetDiffPlotData();
		resetDiffPlotSelectionData();
		removeSelection();
	}

	/**
	 * Image size checker
	 * @warning HACK changeClip method doesn't work in nuke when time of source clip is changed so we have to check size of imgBool all the time
	 */
	bool isImageSizeModified( const OfxPointI& size ) const;
	
	/**
	 * DiffPlot computing
	 */
	void computeFullData( OFX::Clip* clipSrcA, OFX::Clip* clipSrcB, const OfxTime time, const OfxPointD& renderScale, const bool selectionOnly = false );			//compute full data (average/selection/diffPlots)
	void setNbStep( const std::size_t nbStep ) { _vNbStep = nbStep; }
	
	/**
	 * Current time checker
     */
	bool isCurrentTimeModified(const OfxTime time) const;
	
private:
	/*DiffPlot management*/
	void computeDiffPlotBufferData( ChannelData& data, SView& srcAView, SView& srcBView, const OfxTime time, const bool isSelection);

	/*Average management*/
	void computeAverages();		//compute average of each channel

	/*Reset data*/
	void resetDiffPlotData(); //reset data (if size change for example)
	
	void resetDiffPlotSelectionData();	//reset selection data
	void removeSelection();
	
	void resetVectortoZero( DiffPlotMap& v, const std::size_t size ) const;	//reset a specific channel buffer
	
public:
	/// @group Color Mapping By Channel
	/// @{
	ChannelData _channelData;
	ChannelData _selectionData;
	/// @}

	/// @group Color Mapping Color Triplet
	/// @{
	ColorData _colorData;
	/// @}

	bool_2d _imgBool;						//unsigned char 2D (use for display texture on screen)
	OfxTime _currentTime;					//time of the current frame
	std::size_t _vNbStep;					//nbStep for buffers
	bool _isComputing;
	
	bool _isDataInvalid;
	
private:
	OfxPointI _size;						//source clip size
	
};

}
}
}

#endif

