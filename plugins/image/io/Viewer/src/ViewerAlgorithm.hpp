#ifndef _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_

#include <boost/gil/extension/channel.hpp>

#ifdef HAVE_OPENGL_GL_H
#   include <OpenGL/gl.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glut.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace tuttle {
namespace plugin {
namespace viewer {

char *argv[2] = { (char*)"", NULL };
int argc = 1;

int windowID;

int w_out, h_out;

int angle_cam = 60;

void reshape(int w,int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
}

void loadNewUnsignedByteTexture( const unsigned char* data, size_t& width, size_t& height, size_t& channels )
{
    if( channels < 3 || channels > 4 )
        return;
    if( channels == 3 )
        glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );

    if( channels == 4 )
        glTexImage2D( GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

void loadNewShortTexture( const short* data, size_t& width, size_t& height, size_t& channels )
{
    if( channels < 3 || channels > 4 )
        return;
    if( channels == 3 )
        glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_SHORT, data );

    if( channels == 4 )
        glTexImage2D( GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_SHORT, data );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
}

void loadNewFloatTexture( const float* data, size_t& width, size_t& height, size_t& channels )
{
    if( channels < 3 || channels > 4 )
        return;
    if( channels == 3 )
        glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_FLOAT, data );

    if( channels == 4 )
        glTexImage2D( GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_FLOAT, data );

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
    TUTTLE_COUT(k);
    switch (k)
    {
    case '\r':
            glutDestroyWindow(windowID);
            return;
    }
    glutPostRedisplay ();
}

void specialKeyboard( int k, int x, int y)
{
    /*
    switch (k)
    {
        case GLUT_KEY_RIGHT:
            glutDestroyWindow(windowID);
            return;
    }*/
    glutPostRedisplay ();
}

void openGLWindow( const size_t& width, const size_t& height )
{
    glutInit( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA | GLUT_MULTISAMPLE );

    glutInitWindowSize ( width, height );
    glutInitWindowPosition (0,0);
    windowID = glutCreateWindow("TuttleOFX Viewer");



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
