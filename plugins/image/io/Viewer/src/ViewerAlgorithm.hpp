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

char loadedTexture = 0;

float zoom = 1;

const boost::gil::gray8_view_t*   viewGray8;
const boost::gil::gray16_view_t*  viewGray16;
const boost::gil::gray32_view_t*  viewGray32;
const boost::gil::gray32f_view_t* viewGray32f;

const boost::gil::rgb8_view_t*    viewRgb8;
const boost::gil::rgb16_view_t*   viewRgb16;
const boost::gil::rgb32_view_t*   viewRgb32;
const boost::gil::rgb32f_view_t*  viewRgb32f;

const boost::gil::rgba8_view_t*   viewRgba8;
const boost::gil::rgba16_view_t*  viewRgba16;
const boost::gil::rgba32_view_t*  viewRgba32;
const boost::gil::rgba32f_view_t* viewRgba32f;


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

template< typename View>
void loadNewTexture( const View* view )
{
	loadedTexture = 0;
	TUTTLE_COUT("don't find template to load the texture.");
}

template<>
void loadNewTexture( const boost::gil::gray8_view_t* src )
{
	loadedTexture = 1;
	viewGray8 = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE8, src->width(), src->height(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb8_view_t* src )
{
	loadedTexture = 2;
	viewRgb8 = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, src->width(), src->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba8_view_t* src )
{
	loadedTexture = 3;
	viewRgba8 = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	glTexImage2D( GL_TEXTURE_2D, 0, 4, src->width(), src->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray16_view_t* src )
{
	loadedTexture = 4;
	viewGray16 = src;
	short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE16, src->width(), src->height(), 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb16_view_t* src )
{
	loadedTexture = 5;
	viewRgb16 = src;
	short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 3, src->width(), src->height(), 0, GL_RGB, GL_UNSIGNED_SHORT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba16_view_t* src )
{
	loadedTexture = 6;
	viewRgba16 = src;
	short* dataPtr = (short*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 4, src->width(), src->height(), 0, GL_RGBA, GL_UNSIGNED_SHORT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray32_view_t* src )
{
	loadedTexture = 7;
	viewGray32 = src;
	float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, src->width(), src->height(), 0, GL_LUMINANCE, GL_FLOAT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb32_view_t* src )
{
	loadedTexture = 8;
	viewRgb32 = src;
	float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 3, src->width(), src->height(), 0, GL_RGB, GL_FLOAT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba32_view_t* src )
{
	loadedTexture = 9;
	viewRgba32 = src;
	float* dataPtr = (float*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 4, src->width(), src->height(), 0, GL_RGBA, GL_FLOAT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::gray32f_view_t* src )
{
	loadedTexture = 10;
	viewGray32f = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, src->width(), src->height(), 0, GL_LUMINANCE, GL_FLOAT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgb32f_view_t* src )
{
	loadedTexture = 11;
	viewRgb32f = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 3, src->width(), src->height(), 0, GL_RGB, GL_FLOAT, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

template<>
void loadNewTexture( const boost::gil::rgba32f_view_t* src )
{
	loadedTexture = 12;
	viewRgba32f = src;
	unsigned char* dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *src );
	
	glTexImage2D( GL_TEXTURE_2D, 0, 4, src->width(), src->height(), 0, GL_RGBA, GL_FLOAT, dataPtr );
	
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
	glutSwapBuffers();
}

void idle()
{
	
}

void displayInformations()
{
	std::string textureType;
	switch( loadedTexture )
	{
		case 0 : textureType = "undefined type"; break;
		case 1 : textureType = "Gray 8  bits"; break;
		case 2 : textureType = "RGB  8  bits"; break;
		case 3 : textureType = "RGBA 8  bits"; break;
		case 4 : textureType = "Gray 16 bits"; break;
		case 5 : textureType = "RGB  16 bits"; break;
		case 6 : textureType = "RGBA 16 bits"; break;
		case 7 : textureType = "Gray 32 bits"; break;
		case 8 : textureType = "RGB  32 bits"; break;
		case 9 : textureType = "RGBA 32 bits"; break;
		case 10: textureType = "Gray 32 float"; break;
		case 11: textureType = "RGB  32 float"; break;
		case 12: textureType = "RGBA 32 float"; break;
	}
	std::cout << textureType << " " << w_out << "x" << h_out << std::endl;
}

void displayHelp()
{
	std::cout << kViewerHelp << std::endl;
}

void keyboard(unsigned char k, int x, int y)
{
	switch (k)
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
			glutReshapeWindow( w_out, h_out );
			glutPostRedisplay ();
			break;
		case 'h':
			displayHelp();
			break;
		case SPACEBAR:
			glutDestroyWindow(windowID);
			break;
	}
}

void specialKeyboard( int k, int x, int y)
{
	switch (k)
	{
		case GLUT_KEY_RIGHT:
			glutDestroyWindow(windowID);
			return;
		case GLUT_KEY_F1:
			displayHelp();
			return;
	}
}

void mouse  ( int button, int state, int x, int y )
{
	using namespace boost::gil;
	if( state == 0 && button == 0)
	{
		float data[4];
		
		switch( loadedTexture )
		{
			case 0 :
				glReadPixels( x, y, 1, 1, GL_RGBA, GL_FLOAT, &data);
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(9) << data[0] << " " <<
										std::setw(9) << data[1] << " " <<
										std::setw(9) << data[2] << " " <<
										std::setw(9) << data[3] << std::endl;
				break;
			case 1 : // gray 8
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(5) << (int) get_color( viewGray8[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 2 : // rgb 8
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(5) << (int) get_color( viewRgb8[0](x,y), red_t() ) << " " <<
										std::setw(5) << (int) get_color( viewRgb8[0](x,y), green_t() ) << " " <<
										std::setw(5) << (int) get_color( viewRgb8[0](x,y), blue_t() ) << std::endl;
				break;
			case 3 : // rgba 8
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(5) << (int) get_color( viewRgba8[0](x,y), red_t() ) << " " <<
										std::setw(5) << (int) get_color( viewRgba8[0](x,y), green_t() ) << " " <<
										std::setw(5) << (int) get_color( viewRgba8[0](x,y), blue_t() ) << " " <<
										std::setw(5) << (int) get_color( viewRgba8[0](x,y), alpha_t() ) << std::endl;
				break;
			case 4 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(7) << get_color( viewGray16[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 5 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(7) << get_color( viewRgb16[0](x,y), red_t() ) << " " <<
										std::setw(7) << get_color( viewRgb16[0](x,y), green_t() ) << " " <<
										std::setw(7) << get_color( viewRgb16[0](x,y), blue_t() ) << std::endl;
				break;
			case 6 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(7) << get_color( viewRgba16[0](x,y), red_t() ) << " " <<
										std::setw(7) << get_color( viewRgba16[0](x,y), green_t() ) << " " <<
										std::setw(7) << get_color( viewRgba16[0](x,y), blue_t() ) << " " <<
										std::setw(7) << get_color( viewRgba16[0](x,y), alpha_t() ) << std::endl;
				break;
			case 7 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewGray32[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 8 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewRgb32[0](x,y), red_t() ) << " " <<
										std::setw(12) << get_color( viewRgb32[0](x,y), green_t() ) << " " <<
										std::setw(12) << get_color( viewRgb32[0](x,y), blue_t() ) << std::endl;
				break;
			case 9 :
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewRgba32[0](x,y), red_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32[0](x,y), green_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32[0](x,y), blue_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32[0](x,y), alpha_t() ) << std::endl;
				break;
			case 10:
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewGray32f[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 11:
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewRgb32f[0](x,y), red_t() ) << " " <<
										std::setw(12) << get_color( viewRgb32f[0](x,y), green_t() ) << " " <<
										std::setw(12) << get_color( viewRgb32f[0](x,y), blue_t() ) << std::endl;
				break;
			case 12:
				std::cout << "at " <<	std::setw(4) << x << "," <<
										std::setw(4) << y << ": " <<
										std::setw(12) << get_color( viewRgba32f[0](x,y), red_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32f[0](x,y), green_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32f[0](x,y), blue_t() ) << " " <<
										std::setw(12) << get_color( viewRgba32f[0](x,y), alpha_t() ) << std::endl;
				break;
		}
	}
	if( state == 0 && button == 3)
	{
		zoom *= 1.25;
		glutReshapeWindow( w_out * zoom, h_out * zoom );
		glutPostRedisplay ();
	}
	if( state == 0 && button == 4)
	{
		zoom /= 1.25;
		glutReshapeWindow( w_out * zoom, h_out * zoom );
		glutPostRedisplay ();
	}
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
	
	glutKeyboardFunc( keyboard );
	glutSpecialFunc ( specialKeyboard );
	glutMouseFunc   ( mouse );
	//glutJoystickFunc(joystick, 100);
	
	glutReshapeFunc(reshape);
}



}
}
}

#endif
