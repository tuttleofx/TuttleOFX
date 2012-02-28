#ifndef _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_

#include <terry/channel.hpp>

#ifdef HAVE_OPENGL_GL_H
#   include <OpenGL/gl.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glut.h>
#   include <GL/freeglut.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define SPACEBAR 32

namespace tuttle {
namespace plugin {
namespace viewer {

char *argv[2] = { (char*)"", NULL };
int argc = 1;

int windowID;

int w_out, h_out;

int angle_cam = 60;

void reshape(int width,int height)
{
    int w, h;
    int xPos, yPos;
    if( width < height )
    {
	w = width;
	h = 1.0f * w_out / h_out * width;
	xPos = 0.0;
	yPos = 0.5f * (height - h);
    }
    else
    {
	w = 1.0f * w_out / h_out * height;
	h = height;
	xPos = 0.5f * (width - w );
	yPos = 0.0;
    }

    glViewport( xPos, yPos , (GLsizei)w, (GLsizei)h );
    glutReshapeWindow(width, height);
}

template< typename Pixel_t>
void loadNewTexture( const Pixel_t& view  )
{
	TUTTLE_COUT("don't find template to load the texture.");
}

template<>
void loadNewTexture( const boost::gil::gray8_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE8, src.width(), src.height(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb8_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, src.width(), src.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba8_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );
    glTexImage2D( GL_TEXTURE_2D, 0, 4, src.width(), src.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray16_view_t& src )
{
    short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE16, src.width(), src.height(), 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb16_view_t& src )
{
    short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, src.width(), src.height(), 0, GL_RGB, GL_UNSIGNED_SHORT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba16_view_t& src )
{
    short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 4, src.width(), src.height(), 0, GL_RGBA, GL_UNSIGNED_SHORT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray32_view_t& src )
{
    float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, src.width(), src.height(), 0, GL_LUMINANCE, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb32_view_t& src )
{
    float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, src.width(), src.height(), 0, GL_RGB, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba32_view_t& src )
{
    float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 4, src.width(), src.height(), 0, GL_RGBA, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray32f_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, src.width(), src.height(), 0, GL_LUMINANCE, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb32f_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, src.width(), src.height(), 0, GL_RGB, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba32f_view_t& src )
{
    unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( src );

    glTexImage2D( GL_TEXTURE_2D, 0, 4, src.width(), src.height(), 0, GL_RGBA, GL_FLOAT, dataPtr );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

void display()
{
    if( glutGetWindow() == 0 ) return;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    glBegin (GL_QUADS);

    glTexCoord2f( 0, 0 );
    glVertex2f  ( -1, -1 );

    glTexCoord2f( 0, 1 );
    glVertex2f  ( -1, 1 );

    glTexCoord2f( 1, 1 );
    glVertex2f  ( 1, 1 );

    glTexCoord2f( 1, 0 );
    glVertex2f  ( 1, -1 );

    glEnd();
}

void idle()
{
    glutSwapBuffers();
    glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y)
{
    switch (k)
    {
	case '\r':
	    glutDestroyWindow(windowID);
	    return;
	case 27: // ESCAPE key
	    glutDestroyWindow(windowID);
	    return;
	case SPACEBAR:
	    glutDestroyWindow(windowID);
	    return;
    }
    glutPostRedisplay ();
}

void specialKeyboard( int k, int x, int y)
{
    switch (k)
    {
	case GLUT_KEY_RIGHT:
	    glutDestroyWindow(windowID);
	    return;
    }
    glutPostRedisplay ();
}

void openGLWindow( const size_t& width, const size_t& height )
{
    glutInit( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA | GLUT_MULTISAMPLE );
    glutInitWindowSize ( width, height );
    glutInitWindowPosition (0,0);
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION );

    windowID = glutCreateWindow("TuttleOFX Viewer");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    w_out = width;
    h_out = height;

    glutDisplayFunc (display);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc (specialKeyboard);
    //glutJoystickFunc(joystick, 100);

    glutReshapeFunc(reshape);
}



}
}
}

#endif
