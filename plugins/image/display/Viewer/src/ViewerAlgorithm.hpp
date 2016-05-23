#ifndef _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_

#include <terry/channel.hpp>

#include <tuttle/plugin/opengl/gl.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SPACEBAR 32

namespace tuttle
{
namespace plugin
{
namespace viewer
{

char* argv[2] = {(char*)"", NULL};
int argc = 1;

// image properties
struct ImgProperties
{
    const char* data;
    GLint internalFormat;
    size_t width;
    size_t height;
    GLenum format;
    GLenum type;
    size_t component;
};

ImgProperties img;

// opengl properties
int windowID;

// viewing properties
int angle_cam = 60;

// viewing properties - viewport
int w_out, h_out;
float scale = 1.0;
int xMinViewport, yMinViewport;

// viewing properties - zoom
float factorZoom = 1.25;
float currentZoom = 1.0;

// viewing properties - transformations
float x1Quad, x2Quad, y1Quad, y2Quad;
bool flip = false;
bool flop = false;

// mouse moves properties
int x_mouse_ref, y_mouse_ref;

// channel properties
bool showRedChannel = false;
bool showGreenChannel = false;
bool showBlueChannel = false;
bool showAlphaChannel = false;

void reshape(int width, int height)
{
    float w, h, xPos, yPos;

    if((float)w_out / h_out > (float)width / height)
    {
        w = width;
        h = 1.0f * h_out / w_out * (float)width;
        xPos = 0.0;
        yPos = 0.5f * (height - h);
    }
    else
    {
        w = 1.0f * w_out / h_out * (float)height;
        h = height;
        xPos = 0.5f * (width - w);
        yPos = 0.0;
    }

    xMinViewport = xPos;
    yMinViewport = yPos;

    scale = w / w_out;

    glViewport((GLsizei)xPos, (GLsizei)yPos, (GLsizei)w, (GLsizei)h);
    glutReshapeWindow(width, height);
}

void loadNewTexture(const ImgProperties& properties)
{
    // loading texture
    glTexImage2D(GL_TEXTURE_2D, 0, properties.internalFormat, properties.width, properties.height, 0, properties.format,
                 properties.type, properties.data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

void loadNewTexture(const char* data, GLint internalFormat, size_t width, size_t height, GLenum format, GLenum type)
{
    img.data = data;
    img.internalFormat = internalFormat;
    img.width = width;
    img.height = height;
    img.format = format;
    img.type = type;

    switch(img.format)
    {
        case GL_LUMINANCE:
            img.component = 1;
            break;
        case GL_RGB:
            img.component = 3;
            break;
        case GL_RGBA:
            img.component = 4;
            break;
    }

    loadNewTexture(img);
}

void reloadTexture()
{
    loadNewTexture(img);
}

void loadAlphaTexture()
{
    // @TODO find howto pass alpha at luminance
    glTexImage2D(GL_TEXTURE_2D, 0, img.internalFormat, img.width, img.height, 0, img.format, img.type, img.data);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

void setTransfert(float red, float green, float blue, float alpha = 1.f)
{
    switch(img.format)
    {
        case GL_LUMINANCE:
            return;
        case GL_RGB:
            glPixelTransferf(GL_RED_SCALE, red);
            glPixelTransferf(GL_GREEN_SCALE, green);
            glPixelTransferf(GL_BLUE_SCALE, blue);
            break;
        case GL_RGBA:
            glPixelTransferf(GL_RED_SCALE, red);
            glPixelTransferf(GL_GREEN_SCALE, green);
            glPixelTransferf(GL_BLUE_SCALE, blue);
            glPixelTransferf(GL_ALPHA_SCALE, alpha);
            break;
    }
}

void displayChannelTexture(bool& channel, const float red, const float green, const float blue)
{
    ImgProperties p = img;
    if(!channel)
    {
        setTransfert(red, green, blue);
        showRedChannel = false;
        showGreenChannel = false;
        showBlueChannel = false;
        showAlphaChannel = false;
        channel = true;
    }
    else
    {
        setTransfert(1.f, 1.f, 1.f);
        channel = false;
    }
    loadNewTexture(p);

    glutPostRedisplay();
}

void showRedChannelTexture()
{
    displayChannelTexture(showRedChannel, 1.f, 0.f, 0.f);
}

void showGreenChannelTexture()
{
    displayChannelTexture(showGreenChannel, 0.f, 1.f, 0.f);
}

void showBlueChannelTexture()
{
    displayChannelTexture(showBlueChannel, 0.f, 0.f, 1.f);
}

void showAlphaChannelTexture()
{
    if(!showAlphaChannel)
    {
        loadAlphaTexture();
        showRedChannel = false;
        showGreenChannel = false;
        showBlueChannel = false;
        showAlphaChannel = true;
    }
    else
    {
        setTransfert(1.f, 1.f, 1.f, 1.f);
        showAlphaChannel = false;
    }
    glutPostRedisplay();
}

void display()
{
    if(glutGetWindow() == 0)
        return;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);

    float x1 = x1Quad;
    float x2 = x2Quad;

    float y1 = y1Quad;
    float y2 = y2Quad;

    if(flip)
    {
        y1 = -y1;
        y2 = -y2;
    }
    if(flop)
    {
        x1 = -x1;
        x2 = -x2;
    }

    glTexCoord2f(0, 0);
    glVertex2f(x1, y1);

    glTexCoord2f(0, 1);
    glVertex2f(x1, y2);

    glTexCoord2f(1, 1);
    glVertex2f(x2, y2);

    glTexCoord2f(1, 0);
    glVertex2f(x2, y1);

    glEnd();
    glutSwapBuffers();
}

void idle()
{
}

void displayInformations()
{
    std::string textureType;
    switch(img.format)
    {
        case GL_LUMINANCE:
            textureType = "Gray ";
            break;
        case GL_RGB:
            textureType = "RGB  ";
            break;
        case GL_RGBA:
            textureType = "RGBA ";
            break;
    }
    switch(img.type)
    {
        case GL_UNSIGNED_BYTE:
            textureType += "8 bits";
            break;
        case GL_UNSIGNED_SHORT:
            textureType += "16 bits";
            break;
        case GL_FLOAT:
            textureType += "32 float";
            break;
    }
    std::cout << textureType << " " << w_out << "x" << h_out << std::endl;
}

void displayHelp()
{
    std::cout << kViewerHelp << std::endl;
}

void move(float x, float y)
{
    x1Quad += x;
    x2Quad += x;
    y1Quad += y;
    y2Quad += y;
}

void zoom(float factor)
{
    x1Quad *= factor;
    x2Quad *= factor;
    y1Quad *= factor;
    y2Quad *= factor;
}

void mapToImage(int x, int y, int& iX, int& iY)
{
    int mapX, mapY;
    float mx, my;

    mapX = (x - xMinViewport) / scale;
    mapY = (y - yMinViewport) / scale;

    if(!flip)
    {
        mapY = img.height - mapY;
    }

    if(flop)
    {
        mapX = img.width - mapX;
    }

    mx = (float)mapX / (float)img.width * 2.0 - 1.0;
    iX = ((x1Quad - mx) / (currentZoom * 2.0) * (float)img.width * -1.0) + 0.5;

    my = (float)mapY / (float)img.height * 2.0 - 1.0;
    iY = ((y1Quad - my) / (currentZoom * 2.0) * (float)img.height * -1.0) + 0.5;
}

void keyboard(unsigned char k, int x, int y)
{
    bool shift = false;
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    {
        shift = true;
    }

    switch(k)
    {
        case '\r':
            glutDestroyWindow(windowID);
            break;
        case 27: // ESCAPE key
            glutDestroyWindow(windowID);
            break;
        case 'i':
            displayInformations();
            break;
        case 'z':
            glutReshapeWindow(w_out, h_out);
            currentZoom = 1.0;
            x1Quad = -1.0;
            x2Quad = 1.0;
            y1Quad = -1.0;
            y2Quad = 1.0;
            glutPostRedisplay();
            break;
        case 'h':
            displayHelp();
            break;
        case SPACEBAR:
            glutDestroyWindow(windowID);
            break;

        case 'r':
            showRedChannelTexture();
            break;
        case 'g':
            showGreenChannelTexture();
            break;
        case 'b':
            showBlueChannelTexture();
            break;
        case 'a':
            showAlphaChannelTexture();
            break;

        case 'H':
            if(shift)
            {
                flop = !flop;
                glutPostRedisplay();
            }
            break;
        case 'V':
            if(shift)
            {
                flip = !flip;
                glutPostRedisplay();
            }
            break;
    }
}

void specialKeyboard(int k, int x, int y)
{
    switch(k)
    {
        case GLUT_KEY_UP:
            // cursor move
            break;
        case GLUT_KEY_DOWN:
            // cursor move
            break;
        case GLUT_KEY_LEFT:
            // cursor move
            break;
        case GLUT_KEY_RIGHT:
            // cursor move
            break;
        case GLUT_KEY_F1:
            displayHelp();
            break;
    }
}

void mouse(int button, int state, int x, int y)
{
    using namespace boost::gil;
    if(state == 0 && button == 0)
    {
        int iX, iY;

        mapToImage(x, y, iX, iY);

        if(iX < 0 || iY < 0 || iX >= (int)img.width || iY >= (int)img.height)
            return;

        std::cout << "at " << std::setw(4) << iX << "," << std::setw(4) << (int)img.height - iY << ": ";

        for(size_t i = 0; i < img.component; i++)
        {
            size_t idx = (iX + iY * img.width) * img.component + i;
            switch(img.type)
            {
                case GL_UNSIGNED_BYTE:
                {
                    const unsigned char* d = (const unsigned char*)img.data;
                    std::cout << std::setw(5) << (unsigned int)d[idx];
                    break;
                }
                case GL_UNSIGNED_SHORT:
                {
                    const unsigned short* d = (const unsigned short*)img.data;
                    std::cout << std::setw(7) << d[idx];
                    break;
                }
                case GL_FLOAT:
                {
                    const float* d = (const float*)img.data;
                    std::cout << std::setw(10) << d[idx];
                    break;
                }
            }
        }
        std::cout << std::endl;
    }
    if(state == 0 && (button == 3 || button == 4))
    {
        int iX, iY, iX2, iY2;

        mapToImage(x, y, iX, iY);

        if(button == 3)
        {
            currentZoom *= factorZoom;
            zoom(factorZoom);
        }
        else
        {
            currentZoom /= factorZoom;
            zoom(1.0 / factorZoom);
        }

        mapToImage(x, y, iX2, iY2);

        move((currentZoom / img.width * 2) * (iX2 - iX), (currentZoom / img.height * 2) * (iY2 - iY));

        glutPostRedisplay();
    }

    x_mouse_ref = x;
    y_mouse_ref = y;
}

void motion(int x, int y)
{
    float x_diff, y_diff;

    x_diff = (x - x_mouse_ref) / currentZoom;
    y_diff = (y_mouse_ref - y) / currentZoom;

    if(flip)
    {
        y_diff *= -1.0;
    }

    if(flop)
    {
        x_diff *= -1.0;
    }

    move(currentZoom / img.width * 2 * x_diff, currentZoom / img.height * 2 * y_diff);

    x_mouse_ref = x;
    y_mouse_ref = y;

    glutPostRedisplay();
}

void openGLWindow(const size_t& width, const size_t& height)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif

    windowID = glutCreateWindow("TuttleOFX Viewer");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    w_out = width;
    h_out = height;

    x1Quad = -1.0;
    x2Quad = 1.0;
    y1Quad = -1.0;
    y2Quad = 1.0;

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutReshapeFunc(reshape);
}
}
}
}

#endif
