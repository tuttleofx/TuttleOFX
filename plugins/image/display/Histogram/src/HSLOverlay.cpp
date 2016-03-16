#include "HSLOverlay.hpp"

namespace tuttle
{
namespace plugin
{
namespace histogram
{

/*Contructor when Nuke overlay works
HSLOverlay::HSLOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
        _plugin = static_cast<HistogramPlugin*>(_effect);
        _plugin->addRefOverlayData();
}*/

// temporary constructor
HSLOverlay::HSLOverlay(HistogramPlugin* plugin)
{
    _plugin = plugin;
    _plugin->addRefOverlayData();
}

/*Destructor*/
HSLOverlay::~HSLOverlay()
{
    _plugin->releaseOverlayData();
}

/**
 * Display of the RGB histograms data
 */
bool HSLOverlay::draw(const OFX::DrawArgs& args)
{
    bool hasGridBeenDisplayed = false; // grid has not been displayed yet

    // height of the window (image for test)
    // width of the window (image for test)
    OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
    const double step = size.x / (double)(getOverlayData()._data._step - 1);

    double heightH, heightS, heightL;
    heightH = heightS = heightL = size.y;
    if(!(_plugin->_paramDisplayTypeSelection->getValue() == 1))
    {
        // get max of the three channels
        const Number maxH =
            *(std::max_element(getOverlayData()._data._bufferHue.begin(), getOverlayData()._data._bufferHue.end()));
        const Number maxS = *(std::max_element(getOverlayData()._data._bufferSaturation.begin(),
                                               getOverlayData()._data._bufferSaturation.end()));
        const Number maxL = *(std::max_element(getOverlayData()._data._bufferLightness.begin(),
                                               getOverlayData()._data._bufferLightness.end()));

        // Adapt maximal value (3 cases)
        if(maxH > maxS && maxH > maxL) // H is the max
        {
            heightS = maxS * heightH / maxH;
            heightL = maxL * heightH / maxH;
        }
        else if(maxS > maxH && maxS > maxL) // S is the max
        {
            heightH = maxH * heightS / maxS;
            heightL = maxL * heightS / maxS;
        }
        else // L is the max
        {
            heightH = maxH * heightL / maxL;
            heightS = maxS * heightL / maxL;
        }
    }
    float selectionMultiplier = (float)_plugin->_paramSelectionMultiplierSelection->getValue();
    // Display on screen if specified (HSL)
    if(_plugin->_paramOverlayHSelection->getValue()) // HUE CHANNEL
    {
        if(!hasGridBeenDisplayed) // if grid has not been displayed yet
        {
            displayGrid(size.y, size.x); // display grid on screen
            hasGridBeenDisplayed = true; // set grid has already been displayed to true
        }
        displayASpecificHistogram(getOverlayData()._data._bufferHue, getOverlayData()._selectionData._bufferHue, step,
                                  heightH, size.x, redHisto, selectionMultiplier);
        if(getOnlyChannelSelectedHSL() == eSelectedChannelH)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferHue, step, size.x, redHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageHue, averageHisto, size.x, size.y, step); // average bar
            displayHueIndicator(size, kPrecisionHueIndicator); // indicator
        }
    }
    if(_plugin->_paramOverlaySSelection->getValue()) // SATURATION CHANNEL
    {
        if(!hasGridBeenDisplayed) // if grid has not been displayed yet
        {
            displayGrid(size.y, size.x); // display grid on screen
            hasGridBeenDisplayed = true; // set grid has already been displayed to true
        }

        displayASpecificHistogram(getOverlayData()._data._bufferSaturation,
                                  getOverlayData()._selectionData._bufferSaturation, step, heightS, size.x, greenHisto,
                                  selectionMultiplier);
        if(getOnlyChannelSelectedHSL() == eSelectedChannelS)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferSaturation, step, size.x,
                                   greenHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageSaturation, averageHisto, size.x, size.y,
                              step); // average bar
            displaySaturationIndicator(size); // indicator
        }
    }
    if(_plugin->_paramOverlayLSelection->getValue()) // LIGHTNESS CHANNEL
    {
        if(!hasGridBeenDisplayed) // if grid has not been displayed yet
        {
            displayGrid(size.y, size.x); // display grid on screen
            hasGridBeenDisplayed = true; // set grid has already been displayed to true
        }

        displayASpecificHistogram(getOverlayData()._data._bufferLightness, getOverlayData()._selectionData._bufferLightness,
                                  step, heightL, size.x, blueHisto, selectionMultiplier);
        if(getOnlyChannelSelectedHSL() == eSelectedChannelL)
        {
            displaySelectionPoints(getOverlayData()._selectionData._bufferLightness, step, size.x,
                                   blueHisto); // selection points
            displayAverageBar(getOverlayData()._averageData._averageLightness, averageHisto, size.x, size.y,
                              step); // average bar
            displayLightnessIndicator(size); // indicator
        }
    }
    // Display border (separate from histograms to eliminate blending)
    if(_plugin->_paramOverlayHSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferHue, step, heightH, size.x, redHisto); // H
    if(_plugin->_paramOverlaySSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferSaturation, step, heightS, size.x, greenHisto); // S
    if(_plugin->_paramOverlayLSelection->getValue())
        displayASpecificHistogramBorder(getOverlayData()._data._bufferLightness, step, heightL, size.x, blueHisto); // L
    return true;
}

/**
 * Permits to know if there is only one RGB channel selected
 * @return "R", "G", "B" if there is only one channel selected,  "none" or "more" else
 */
ESelectedChannelHSL HSLOverlay::getOnlyChannelSelectedHSL() const
{
    // get all of the checkbox states HSL
    bool H = _plugin->_paramOverlayHSelection->getValue(); // check box H value
    bool S = _plugin->_paramOverlaySSelection->getValue(); // check box S value
    bool L = _plugin->_paramOverlayLSelection->getValue(); // check box L value

    if(H || S || L)
    {
        if(H && !S && !L)
            return eSelectedChannelH; // Only hue is selected
        if(!H && S && !L)
            return eSelectedChannelS; // Only saturation is selected
        if(!H && !S && L)
            return eSelectedChannelL; // Only lightness is selected
        return eSelectedChannelMoreHSL; // More than one HSL channel are selected
    }
    return eSelectedChannelNoneHSL; // None HSL channel is selected
}

/**
 * Get overlay data from plugin
 * @return
 */
OverlayData& HSLOverlay::getOverlayData()
{
    return _plugin->getOverlayData();
}

/**
 * Display grid on screen
 */
void HSLOverlay::displayGrid(float height, float width)
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
    float baseY = 0; // initialize y to O
    float baseX = 0; // initialize x to 0
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
    baseY = 0; // initialize y to O
    baseX = 0; // initialize x to 0
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
