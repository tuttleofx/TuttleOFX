#ifndef DIFFPLOTDISPLAY_HPP
#define	DIFFPLOTDISPLAY_HPP

#include "DiffPlotDefinitions.hpp"
#include "OverlayData.hpp"

namespace tuttle {
namespace plugin {
namespace diffPlot {


/**
 *Display grid on screen
 */
void displayGrid(float height, float width);

void plotChannelMapping( const DiffPlotMap& data, const DiffPlotMap& selectionData, const int step, const int height, const int width, const double selectionMultiplier );

void displayHorizontalIndicator(const OfxPointI size);

}
}
}

#endif
