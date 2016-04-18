#ifndef HISTOGRAMKEYERHISTOGRAMDISPLAY_HPP
#define HISTOGRAMKEYERHISTOGRAMDISPLAY_HPP

#include "HistogramKeyerDefinitions.hpp"
#include "OverlayData.hpp"

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

void displayASpecificHistogram(const HistogramVector& v, const HistogramVector& selection_v, const double step,
                               const double height, const double width, const HistogramColor color,
                               float selectionMultiplier);
void displayASpecificHistogramBorder(const HistogramVector& v, const double step, const double height, const double width,
                                     const HistogramColor color);

void displaySelectionPoints(const HistogramVector& selection_v, const double step, const double width,
                            const HistogramColor color);

void displayRedIndicator(const OfxPointI size);   // display red indicator (center)
void displayGreenIndicator(const OfxPointI size); // display green indicator (center)
void displayBlueIndicator(const OfxPointI size);  // display blue indicator (center)

void displayLightnessIndicator(const OfxPointI size);                      // display lightness indicator (center)
void displaySaturationIndicator(const OfxPointI size);                     // display saturation indicator (center)
void displayHueIndicator(const OfxPointI size, int precisionHueIndicator); // display hue indicator (center)

void displayAverageBar(const int average, HistogramColor color, const int width, const int height, const double step);
}
}
}

#endif
