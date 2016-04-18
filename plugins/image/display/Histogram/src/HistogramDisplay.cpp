#include "HistogramDisplay.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <boost/gil/extension/color/hsv.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

/**
 * Display the given vector on screen
 * @param v specific vector to display on overlay
 * @param step step of the display
 * @param height maximal height
 * @param width maximal width
 * @param color color used to display
 */
void displayASpecificHistogram(const HistogramVector& v, const HistogramVector& selection_v, const double step,
                               const double height, const double width, const HistogramColor color,
                               float selectionMultiplier)
{
    if(v.size())
    {
        // maximum data in the current channel vector
        const Number max_value = *(std::max_element(v.begin(), v.end()));
        const float ratio = height / max_value;
        // OpenGL 2.X
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE); // additive blending active
        glColor3f(color._colorFill.r, color._colorFill.g, color._colorFill.b);
        // Display option
        glBegin(GL_QUAD_STRIP);
        double base_step = 0.0; // first point
        for(unsigned int i = 0; i < v.size(); ++i)
        {
            const float value = (float)(v.at(i) * ratio);
            float selection_value = (float)(selection_v.at(i) * ratio);
            selection_value *= selectionMultiplier;
            if(selection_value > value) // if selection value is bigger than normal value replace it
                selection_value = value;
            glVertex2f((float)(base_step), (float)(value));
            glVertex2f((float)(base_step), (float)(selection_value));
            base_step += step;
        }
        glVertex2f((float)width, 0.0f); // last point
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
void displayASpecificHistogramBorder(const HistogramVector& v, const double step, const double height, const double width,
                                     const HistogramColor color)
{
    // Draw the border line
    glBegin(GL_LINE_STRIP);
    // maximum data in the current channel vector
    const Number max_value = *(std::max_element(v.begin(), v.end()));
    const float ratio = height / max_value;
    double base_step = 0.0;
    glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
    for(unsigned int i = 0; i < v.size(); ++i)
    {
        const float value = (float)(v.at(i) * ratio);
        glVertex2f((float)(base_step), float(value));
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
void displaySelectionPoints(const HistogramVector& selection_v, const double step, const double width,
                            const HistogramColor color)
{
    glBegin(GL_POINTS);
    double base_step = 0.0;
    glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
    for(unsigned int i = 0; i < selection_v.size(); ++i)
    {
        if(selection_v.at(i) != 0)
        {
            glVertex2f((float)(base_step), -10.0f);
        }
        base_step += step;
    }
    glEnd();
}

/**
 * display the red indicator (openGL)
 * @param size size of the source clip
 */
void displayRedIndicator(const OfxPointI size)
{
    double heightIndicator = (size.y - 10) * 0.07;
    double width = size.x;

    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);
    glBegin(GL_QUADS);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f((float)width, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f((float)width, (float)(-heightIndicator));

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, (float)(-heightIndicator));
    glEnd();
    glPopMatrix();
}

/**
 * display the green indicator (openGL)
 * @param size size of the source clip
 */
void displayGreenIndicator(const OfxPointI size)
{
    double heightIndicator = size.y * 0.07;
    double width = size.x;

    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);
    glBegin(GL_QUADS);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f((float)width, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f((float)width, (float)(-heightIndicator));

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, (float)(-heightIndicator));
    glEnd();
    glPopMatrix();
}

/**
 * display the green indicator (openGL)
 * @param size size of the source clip
 */
void displayBlueIndicator(const OfxPointI size)
{
    double heightIndicator = size.y * 0.07;
    double width = size.x;

    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);

    glBegin(GL_QUADS);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f((float)width, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f((float)width, (float)(-heightIndicator));

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, (float)(-heightIndicator));
    glEnd();
    glPopMatrix();
}

/**
 * display the lightness indicator (openGL)
 * @param size size of the source clip
 */
void displayLightnessIndicator(const OfxPointI size)
{
    double heightIndicator = size.y * 0.07;
    double width = size.x;

    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);
    glBegin(GL_QUADS);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f((float)width, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f((float)width, (float)(-heightIndicator));

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, (float)(-heightIndicator));
    glEnd();
    glPopMatrix();
}

/**
 * display the saturation indicator (openGL)
 * @param size size of the source clip
 */
void displaySaturationIndicator(const OfxPointI size)
{
    double heightIndicator = size.y * 0.07;
    double width = size.x;

    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);
    glBegin(GL_QUADS);

    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(0.0f, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f((float)width, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f((float)width, (float)(-heightIndicator));

    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(0.0f, (float)(-heightIndicator));
    glEnd();
    glPopMatrix();
}

/**
 * display the hue indicator (openGL)
 * @param size size of the source clip
 * @param precisionHueIndicator number of step of the indicator
 */
void displayHueIndicator(const OfxPointI size, int precisionHueIndicator)
{
    static const float length = 360.0;
    const double heightIndicator = size.y * 0.07;
    const double width = size.x;
    glPushMatrix();
    glTranslated(0.0f, -20.0f, 0.0f);
    glBegin(GL_QUAD_STRIP);
    const float ratio = length / (float)(kPrecisionHueIndicator - 1.0);
    float hue = 0.0f;
    for(std::ssize_t i = 0; i < precisionHueIndicator; ++i)
    {
        boost::gil::hsv32f_pixel_t hsl_pix;      // declare a HSL pixel
        boost::gil::rgb32f_pixel_t pix(0, 0, 0); // declare a RGB pixel (don't need alpha)
        hsl_pix[0] = (float)(hue / length);      // fill up HSL pixel
        hsl_pix[1] = (float)(1.0);
        hsl_pix[2] = (float)(1.0);

        color_convert(hsl_pix, pix);       // convert HSL=>RGB for OpenGl
        glColor3f(pix[0], pix[1], pix[2]); // generate openGL color

        glVertex2f(hue * (width / length), -0.0f);
        glVertex2f(hue * (width / length), (float)(-heightIndicator)); // draw quad
        hue += ratio;
    }
    glEnd();
    glPopMatrix();
}

/**
 * Display an average bar on screen
 * @param average average of the current channel
 * @param color color chosed to display
 * @param width width of the image (1 into Nuke overlay)
 * @param height height of the image
 * @param step (image width/nbstep in histograms buffers)
 */
void displayAverageBar(const int average, HistogramColor color, const int width, const int height, const double step)
{
    if(average == 0)
        return;
    float _average = average * step;
    glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
    glBegin(GL_LINES);
    glVertex2i((float)(_average), 0.0f);
    glVertex2i((float)(_average), (float)height);
    glEnd();
}
}
}
}
