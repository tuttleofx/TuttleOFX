#include "HistogramKeyerHistogramDisplay.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <boost/gil/extension/color/hsv.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	
HistogramKeyerHistogramDisplay::HistogramKeyerHistogramDisplay(HistogramKeyerPlugin* plugin) 
{
	_plugin = plugin;
	_isAdaptedToHeight = false;
}

/**
 * Display of the RGB histograms data
 * @param histogramBuffer container of data
 * @param time current time
 * @param heightAdapted is height adapted (bool)
 */
void HistogramKeyerHistogramDisplay::displayHistogramOnScreenRGB( const HistogramBufferData& histogramBuffer,const HistogramBufferData& histogramSelectionBuffer, const OfxTime time) const
{	
	//height of the window (image for test)
	//width of the window (image for test)
	const OfxPointI size = _plugin->_clipSrc->getPixelRodSize(time);
    const double step = size.x / (double)(histogramBuffer._step -1);
	double heightR, heightG,heightB;
    heightR = heightG = heightB = size.y;
    if(!(this->_isAdaptedToHeight))
    {
		//get max of the three channels
		Number maxR = *(std::max_element(this->_plugin->_data._bufferRed.begin(),this->_plugin->_data._bufferRed.end()));
        Number maxG = *(std::max_element(this->_plugin->_data._bufferGreen.begin(),this->_plugin->_data._bufferGreen.end()));
        Number maxB = *(std::max_element(this->_plugin->_data._bufferBlue.begin(),this->_plugin->_data._bufferBlue.end()));
            
        if(maxR > maxG && maxR > maxB)
        {
			heightG = maxG*heightR/maxR;
            heightB = maxB*heightR/maxR;
        }
        else if(maxG > maxR && maxG > maxB)
        {
            heightR = maxR*heightG/maxG;
            heightB = maxB*heightG/maxG;
        }
        else
        {
            heightR = maxR*heightB/maxB;
            heightG = maxG*heightB/maxB;
        }
    }
	
	//display on screen if specified (RGB)
	if(_plugin->_paramOverlayRSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferRed,histogramSelectionBuffer._bufferRed,step,heightR,size.x,redHisto);
		if(getOnlyChannelSelectedRGB()=="R")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferRed,step,size.x,redHisto);
			displayRedIndicator(size);
		}
	}
	if(_plugin->_paramOverlayGSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferGreen,histogramSelectionBuffer._bufferGreen,step,heightG,size.x,greenHisto);
		if(getOnlyChannelSelectedRGB()=="G")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferGreen,step,size.x,greenHisto);
			displayGreenIndicator(size);
		}
	}
	if(_plugin->_paramOverlayBSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferBlue,histogramSelectionBuffer._bufferBlue,step,heightB,size.x,blueHisto);
		if(getOnlyChannelSelectedRGB()=="B")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferBlue,step,size.x,blueHisto);
			displayBlueIndicator(size);
		}
	}
	//Display border (separate from histograms to eliminate blending)
	if(_plugin->_paramOverlayRSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferRed,step,heightR,size.x,redHisto);		//R
	if(_plugin->_paramOverlayGSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferGreen,step,heightG,size.x,greenHisto);	//G
	if(_plugin->_paramOverlayBSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferBlue,step,heightB,size.x,blueHisto);		//B

}

/**
 * Display of the HSL histograms data
 * @param histogramBuffer container of data
 * @param time current time
 * @param heightAdapted is height adapted (bool)
 */
void HistogramKeyerHistogramDisplay::displayHistogramOnScreenHSL( const HistogramBufferData& histogramBuffer,const HistogramBufferData& histogramSelectionBuffer, const OfxTime time) const
{
	//height of the window (image for test)
	//width of the window (image for test)
	const OfxPointI size = _plugin->_clipSrc->getPixelRodSize(time);
    const double step = size.x / (double)(histogramBuffer._step -1);
        
    double heightH, heightS,heightL;
    heightH = heightS = heightL = size.y;
    if(!(this->_isAdaptedToHeight))
    {
		//get max of the three channels
        const Number maxH = *(std::max_element(_plugin->_data._bufferHue.begin(),_plugin->_data._bufferHue.end()));
        const Number maxS = *(std::max_element(_plugin->_data._bufferSaturation.begin(),_plugin->_data._bufferSaturation.end()));
        const Number maxL = *(std::max_element(_plugin->_data._bufferLightness.begin(),_plugin->_data._bufferLightness.end()));
            
		//Adapt maximal value (3 cases)
        if(maxH > maxS && maxH > maxL)
        {
			heightS = maxS*heightH/maxH;
            heightL = maxL*heightH/maxH;
        }
        else if(maxS > maxH && maxS > maxL)
        {
            heightH = maxH*heightS/maxS;
            heightL = maxL*heightS/maxS;
        }
        else
        {
            heightH = maxH*heightL/maxL;
            heightS = maxS*heightL/maxL;
        }
    }
	
	//Display on screen if specified (HSL)
	if(_plugin->_paramOverlayHSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferHue,histogramSelectionBuffer._bufferHue,step,heightH,size.x,redHisto);
		if(getOnlyChannelSelectedHSL()=="H")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferHue,step,size.x,redHisto);
			displayHueIndicator(size);
		}
	}
	if(_plugin->_paramOverlaySSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferSaturation,histogramSelectionBuffer._bufferSaturation,step,heightS,size.x,greenHisto);
		if(getOnlyChannelSelectedHSL()=="S")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferSaturation,step,size.x,greenHisto);
			displaySaturationIndicator(size);
		}
	}
	if(_plugin->_paramOverlayLSelection->getValue())
	{
		displayASpecificHistogram(histogramBuffer._bufferLightness,histogramSelectionBuffer._bufferLightness,step,heightL,size.x,blueHisto);
		if(getOnlyChannelSelectedHSL()=="L")
		{
			displaySelectionPoints(histogramSelectionBuffer._bufferLightness,step,size.x,blueHisto);
			displayLightnessIndicator(size);
		}
	}
	//Display border (separate from histograms to eliminate blending)
	if(_plugin->_paramOverlayHSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferHue,step,heightH,size.x,redHisto);			//H
	if(_plugin->_paramOverlaySSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferSaturation,step,heightS,size.x,greenHisto);	//S
	if(_plugin->_paramOverlayLSelection->getValue())
		displayASpecificHistogramBorder(histogramBuffer._bufferLightness,step,heightL,size.x,blueHisto);	//L
}

/**
 * Display the given vector on screen
 * @param v specific vector to display on overlay
 * @param step step of the display
 * @param height maximal height
 * @param width maximal width
 * @param color color used to display
 */
void HistogramKeyerHistogramDisplay::displayASpecificHistogram(const std::vector<Number>& v,const std::vector<Number>& selection_v, const double step, const double height, const double width, const HistogramColor color) const
{
	if(v.size())
	{
		//maximum data in the current channel vector
		const Number max_value = *(std::max_element(v.begin(),v.end()));
		const float ratio = height/max_value;
		//OpenGL 2.X
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE); //additive blending active
		glColor3f(color._colorFill.r, color._colorFill.g, color._colorFill.b);
		//Display option
		glBegin( GL_QUAD_STRIP );
		double base_step = 0.0;
		for(unsigned int i=0; i<v.size(); ++i)
		{
			const float value = (float)(v.at(i)*ratio);
			float selection_value = (float)(selection_v.at(i)*ratio);

			selection_value*= _plugin->_selectionMultiplierSelection->getValue(); 
			if(selection_value > value)
				selection_value = value;
			///@todo : remove condition when Nuke curves overlay works
			if(_translateHSL)
			{
				float translationHSL = (float)(width+kTranslationHSL);
				glVertex2f((float)(base_step+translationHSL), (float)(value));
				glVertex2f((float)(base_step+translationHSL), (float)(selection_value));
			}
			else
			{
				glVertex2f((float)base_step,(float)(value));
				glVertex2f((float)base_step,(float)(selection_value));
			}
			base_step += step;
		}
		glVertex2f((float)width,0.0f);
		glEnd();
		glDisable(GL_BLEND);
	}
}

/**
 * Display the given vector on screen (only border)
 * @param v specific vector to display on overlay
 * @param step step of the display
 * @param height maximal height
 * @param width maximal width
 * @param color color used to display
 */
void HistogramKeyerHistogramDisplay::displayASpecificHistogramBorder(const std::vector<Number>& v, const double step, const double height, const double width, const HistogramColor color) const
{
	//Draw the border line
	glBegin( GL_LINE_STRIP );
	//maximum data in the current channel vector
	const Number max_value = *(std::max_element(v.begin(),v.end()));
	const float ratio = height/max_value;
	double base_step = 0.0;
	glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
	for(unsigned int i=0; i<v.size(); ++i)
	{
		const float value = (float)(v.at(i)*ratio);
		///@todo : remove condition when Nuke curves overlay works
		if(_translateHSL)
		{
			float translationHSL = (float)(width+kTranslationHSL);
			glVertex2f((float)(base_step+translationHSL), float(value));
		}
		else
		{
			glVertex2f( (float)base_step, float(value));
		}
		base_step += step;
	}
	glEnd();
}

/**
 * display selection points under the histograms
 * @param selection_v buffer which contains all of the selection points
 * @param step step for display
 * @param width width of the source clip
 * @param color color using for display
 */
void HistogramKeyerHistogramDisplay::displaySelectionPoints(const std::vector<Number>& selection_v, const double step, const double width, const HistogramColor color) const
{
	glBegin( GL_POINTS );
	double base_step = 0.0;
	glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
	for(unsigned int i=0; i<selection_v.size(); ++i)
	{
		if(selection_v.at(i) != 0)
		{
			///@todo : remove condition when Nuke curves overlay works
			if(_translateHSL)
			{
				float translationHSL = (float)(width+kTranslationHSL);
				glVertex2f((float)(base_step+translationHSL), -10.0f);
			}
			else
				glVertex2f( (float)base_step, -10.0f);
		}	
		base_step += step;
	}
	glEnd();
}

/**
 * display the red indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displayRedIndicator(const OfxPointI size) const
{
	double heightIndicator = (size.y-10)*0.07;
	double width = size.x;
	
	glPushMatrix();
	glTranslated(0.0f,-20.0f,0.0f);
	glBegin( GL_QUADS);
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,0.0f);
	
	glColor3f(1.0f, 0.0f, 0.0f);	
	glVertex2f((float)width,0.0f);
	
	glColor3f(1.0f, 0.0f, 0.0f);	
	glVertex2f((float)width,(float)(-heightIndicator));
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,(float)(-heightIndicator));
	glEnd();
	glPopMatrix();
	
}

/**
 * display the green indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displayGreenIndicator(const OfxPointI size) const
{
	double heightIndicator = size.y*0.07;
	double width = size.x;
	
	glPushMatrix();
	glTranslated(0.0f,-20.0f,0.0f);
	glBegin( GL_QUADS);
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,0.0f);
	
	glColor3f(0.0f, 1.0f, 0.0f);	
	glVertex2f((float)width,0.0f);
	
	glColor3f(0.0f, 1.0f, 0.0f);	
	glVertex2f((float)width,(float)(-heightIndicator));
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,(float)(-heightIndicator));
	glEnd();
	glPopMatrix();
	
}

/**
 * display the green indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displayBlueIndicator(const OfxPointI size) const
{
	double heightIndicator = size.y*0.07;
	double width = size.x;
	
	glPushMatrix();
	glTranslated(0.0f,-20.0f,0.0f);
	glBegin( GL_QUADS);
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,0.0f);
	
	glColor3f(0.0f, 0.0f, 1.0f);	
	glVertex2f((float)width,0.0f);
	
	glColor3f(0.0f, 0.0f, 1.0f);	
	glVertex2f((float)width,(float)(-heightIndicator));
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,(float)(-heightIndicator));
	glEnd();
	glPopMatrix();
}

/**
 * display the lightness indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displayLightnessIndicator(const OfxPointI size) const
{
	double heightIndicator = size.y*0.07;
	double width = size.x;
	
	glPushMatrix();
	///@todo : remove condition when Nuke curves overlay works
	if(_translateHSL)
	{
		float translationHSL = (float)(size.x+kTranslationHSL);
		glTranslated(translationHSL,-20.0f,0.0f);
	}
	else
	{
		glTranslated(0.0f,-20.0f,0.0f);
	}
	glBegin( GL_QUADS);
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,0.0f);
	
	glColor3f(1.0f, 1.0f, 1.0f);	
	glVertex2f((float)width,0.0f);
	
	glColor3f(1.0f, 1.0f, 1.0f);	
	glVertex2f((float)width,(float)(-heightIndicator));
	
	glColor3f(0.0f, 0.0f, 0.0f);	
	glVertex2f(0.0f,(float)(-heightIndicator));
	glEnd();
	glPopMatrix();
}

/**
 * display the saturation indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displaySaturationIndicator(const OfxPointI size) const
{
	double heightIndicator = size.y*0.07;
	double width = size.x;
	
	glPushMatrix();
	///@todo : remove condition when Nuke curves overlay works
	if(_translateHSL)
	{
		float translationHSL = (float)(size.x+kTranslationHSL);
		glTranslated(translationHSL,-20.0f,0.0f);
	}
	else
	{
		glTranslated(0.0f,-20.0f,0.0f);
	}
	glBegin( GL_QUADS);
	
	glColor3f(0.5f, 0.5f, 0.5f);	
	glVertex2f(0.0f,0.0f);
	
	glColor3f(1.0f, 0.0f, 0.0f);	
	glVertex2f((float)width,0.0f);
	
	glColor3f(1.0f, 0.0f, 0.0f);	
	glVertex2f((float)width,(float)(-heightIndicator));
	
	glColor3f(0.5f, 0.5f, 0.5f);	
	glVertex2f(0.0f,(float)(-heightIndicator));
	glEnd();
	glPopMatrix();
}

/**
 * display the hue indicator (openGL)
 * @param size size of the source clip
 */
void HistogramKeyerHistogramDisplay::displayHueIndicator(const OfxPointI size) const
{
	static const float length = 360.0;
	
	const double heightIndicator = size.y*0.07;
	const double width = size.x;
	
	glPushMatrix();
	///@todo : remove condition when Nuke curves overlay works
	if(_translateHSL)
	{
		float translationHSL = (float)(size.x+kTranslationHSL);
		glTranslated(translationHSL,-20.0f,0.0f);
	}
	else
	{
		glTranslated(0.0f,-20.0f,0.0f);
	}
	glBegin( GL_QUAD_STRIP);
	const float ratio = length / (float)(kPrecisionHueIndicator-1.0);
	float hue = 0.0f;
	for(unsigned int i=0; i< kPrecisionHueIndicator; ++i)
	{
		boost::gil::hsv32f_pixel_t hsl_pix;
		boost::gil::rgb32f_pixel_t pix;
		hsl_pix[0] = (float)(hue/length);
		hsl_pix[1] = (float)(1.0);
		hsl_pix[2] = (float)(1.0);
		
		color_convert(hsl_pix, pix); //convert HSL=>RGB for OpenGl
		Color toApply = { pix[0], pix[1], pix[2] };
		
		glColor3f(toApply.r,toApply.g,toApply.b);
		glVertex2f(hue*(width/length),-0.0f);	
		glVertex2f(hue*(width/length),(float)(-heightIndicator));
		hue+=ratio;
	}
	glEnd();
	glPopMatrix();
}

/**
 * Permits to know if there is only one RGB channel selected
 * @return "R", "G", "B" if there is only one channel selected, "notOnlyOneSelected" else
 */
std::string HistogramKeyerHistogramDisplay::getOnlyChannelSelectedRGB() const
{
	//get all of the checkbox states RGB
	bool R = _plugin->_paramOverlayRSelection->getValue();
	bool G = _plugin->_paramOverlayGSelection->getValue();
	bool B = _plugin->_paramOverlayBSelection->getValue();
	
	if(R||G||B)
	{
		if(R && !G && !B)
			return "R";
		if(!R && G && !B)
			return "G";
		if(!R && !G && B)
			return "B";
	}
	return "notOnlyOneSelected";
}

/**
 * Permits to know if there is only one HSL channel selected
 * @return "H", "S", "L" if there is only one channel selected, "notOnlyOneSelected" else
 */
std::string HistogramKeyerHistogramDisplay::getOnlyChannelSelectedHSL() const
{
	//HSL
	bool H = _plugin->_paramOverlayHSelection->getValue();
	bool S = _plugin->_paramOverlaySSelection->getValue();
	bool L = _plugin->_paramOverlayLSelection->getValue();
		
	if(H||S||L)
	{
		if(H && !S && !L)
			return "H";
		if(!H && S && !L)
			return "S";
		if(!H && !S && L)
			return "L";
	}
	return "notOnlyOneSelected";
}

}
}
}