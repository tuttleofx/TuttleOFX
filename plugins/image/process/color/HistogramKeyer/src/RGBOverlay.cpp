#include "RGBOverlay.hpp"

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

/*Contructor (when Nuke works)
RGBOverlay::RGBOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
        _plugin = static_cast<HistogramKeyerPlugin*>(_effect);
        _plugin->addRefOverlayData();
}*/

// tomporary constructor
RGBOverlay::RGBOverlay(HistogramKeyerPlugin* plugin)
{
    _plugin = plugin;
    _plugin->addRefOverlayData();
}

/*Destructor*/
RGBOverlay::~RGBOverlay()
{
    _plugin->releaseOverlayData();
}

/**
 * Display of the RGB histograms data
 */
bool RGBOverlay::draw(const OFX::DrawArgs& args)
{
    _isGriddisplay = false;
    // height of the window (image for test)
    // width of the window (image for test)
    const OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
    const double step = size.x / (double)(getOverlayData()._data._step - 1);
    double heightR, heightG, heightB;
    heightR = heightG = heightB = size.y;

    if(!(_plugin->_paramDisplayTypeSelection->getValue() == 1))
    {
        // get max of the three channels
        Number maxR =
            *(std::max_element(getOverlayData()._data._bufferRed.begin(), getOverlayData()._data._bufferRed.end()));
        Number maxG =
            *(std::max_element(getOverlayData()._data._bufferGreen.begin(), getOverlayData()._data._bufferGreen.end()));
        Number maxB =
            *(std::max_element(getOverlayData()._data._bufferBlue.begin(), getOverlayData()._data._bufferBlue.end()));

        if(maxR > maxG && maxR > maxB) // R is the max
        {
            heightG = maxG * heightR / maxR;
            heightB = maxB * heightR / maxR;
        }
        else if(maxG > maxR && maxG > maxB) // G is the max
        {
            heightR = maxR * heightG / maxG;
            heightB = maxB * heightG / maxG;
        }
        else // B is the max
        {
            heightR = maxR * heightB / maxB;
            heightG = maxG * heightB / maxB;
        }
    }
    float selectionMultiplier =
        (float)_plugin->_paramSelectionMultiplierSelection->getValue(); // get selection multiplier from plugin
    // display on screen if specified (RGB)

    if(_plugin->_paramOverlayRSelection->getValue()) // RED CHANNEL
    {
        if(!_isGriddisplay) // if grid has not been already displayed
        {
            displayGrid(size.y, size.x); // display grid
            _isGriddisplay = true;       // set display grid to true
        }

        displayASpecificHistogram(getOverlayData()._data._bufferRed, getOverlayData()._selectionData._bufferRed, step,
                                  heightR, size.x, redHisto, selectionMultiplier);
        if(getOnlyChannelSelectedRGB() == eSelectedChannelR)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferRed, step, size.x, redHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageRed, averageHisto, size.x, size.y, step); // average bar
            displayRedIndicator(size);                                                                        // indicator
        }
    }
    if(_plugin->_paramOverlayGSelection->getValue()) // GREEN CHANNEL
    {
        if(!_isGriddisplay) // if grid has not been already displayed
        {
            displayGrid(size.y, size.x); // display grid
            _isGriddisplay = true;       // set display grid to true
        }

        displayASpecificHistogram(getOverlayData()._data._bufferGreen, getOverlayData()._selectionData._bufferGreen, step,
                                  heightG, size.x, greenHisto, selectionMultiplier);
        if(getOnlyChannelSelectedRGB() == eSelectedChannelG)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferGreen, step, size.x,
                                   greenHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageGreen, averageHisto, size.x, size.y, step); // average
                                                                                                                // bar
            displayGreenIndicator(size);                                                                        // indicator
        }
    }
    if(_plugin->_paramOverlayBSelection->getValue()) // BLUE CHANNEL
    {
        if(!_isGriddisplay) // if grid has not been already displayed
        {
            displayGrid(size.y, size.x); // display grid
            _isGriddisplay = true;       // set display grid to true
        }

        displayASpecificHistogram(getOverlayData()._data._bufferBlue, getOverlayData()._selectionData._bufferBlue, step,
                                  heightB, size.x, blueHisto, selectionMultiplier);
        if(getOnlyChannelSelectedRGB() == eSelectedChannelB)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferBlue, step, size.x, blueHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageBlue, averageHisto, size.x, size.y, step); // average bar
            displayBlueIndicator(size);                                                                        // indicator
        }
    }
    // Display border (separate from histograms to eliminate blending)
    if(_plugin->_paramOverlayRSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferRed, step, heightR, size.x, redHisto); // R
    if(_plugin->_paramOverlayGSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferGreen, step, heightG, size.x, greenHisto); // G
    if(_plugin->_paramOverlayBSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferBlue, step, heightB, size.x, blueHisto); // B
    return true;
}

/**
 * Permits to know if there is only one RGB channel selected
 * @return "R", "G", "B" if there is only one channel selected,  "none" or "more" else
 */
ESelectedChannelRGB RGBOverlay::getOnlyChannelSelectedRGB() const
{
    // get all of the checkbox states RGB
    bool R = _plugin->_paramOverlayRSelection->getValue(); // check box R value
    bool G = _plugin->_paramOverlayGSelection->getValue(); // check box G value
    bool B = _plugin->_paramOverlayBSelection->getValue(); // check box B value

    if(R || G || B)
    {
        if(R && !G && !B)
            return eSelectedChannelR; // Only red is selected
        if(!R && G && !B)
            return eSelectedChannelG; // Only green is selected
        if(!R && !G && B)
            return eSelectedChannelB;   // Only blue is selected
        return eSelectedChannelMoreRGB; // More than one RGB channel are selected
    }
    return eSelectedChannelNoneRGB; // None channel is selected
}

/**
 * Get overlay data from plugin
 * @return
 */
OverlayData& RGBOverlay::getOverlayData()
{
    return _plugin->getOverlayData();
}

/**
 *Display grid on screen
 */
void RGBOverlay::displayGrid(float height, float width)
{
    glBegin(GL_LINES);
    // draw standard reference
    glColor3f(.9f, .9f, .9f); // white color
    glVertex2f(-0.1f, 0);
    glVertex2f(-0.1f, height); // Y axis
    glVertex2f(0, -0.1f);
    glVertex2f(width, -0.1f); // X axis

    // compute step X and Y
    float stepY = (float)(height / 10.0f);
    float stepX = (float)(width / 10.0f);
    float stepYm = (float)(height / 20.0f);
    float stepXm = (float)(width / 20.0f);

    // drawing minor grid
    glColor3f(.2f, .2f, .2f); // gray color
    float baseY = 0;          // initialize y to O
    float baseX = 0;          // initialize x to 0
    for(unsigned int i = 0; i < 20; ++i)
    {
        baseY += stepYm; // update Y position
        baseX += stepXm; // update X position
        glVertex2f(-0.1f, baseY);
        glVertex2f(width, baseY); // draw Y axis
        glVertex2f(baseX, -0.1f);
        glVertex2f(baseX, height); // draw X axis
    }

    // drawing major grid
    glColor3f(.5f, .5f, .5f); // gray color
    baseY = 0;                // initialize y to O
    baseX = 0;                // initialize x to 0
    for(unsigned int i = 0; i < 10; ++i)
    {
        baseY += stepY; // update Y position
        baseX += stepX; // update X position
        glVertex2f(-0.1f, baseY);
        glVertex2f(width, baseY); // draw Y axis
        glVertex2f(baseX, -0.1f);
        glVertex2f(baseX, height); // draw X axis
    }

    // draw grid
    glEnd();
}
}
}
}
