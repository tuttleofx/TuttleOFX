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

bool showRedChannel = false;
bool showGreenChannel = false;
bool showBlueChannel = false;
bool showAlphaChannel = false;

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

void reloadTexture( )
{
	unsigned char* dataPtr = NULL;
	
	GLint	internalFormat = 1;
	size_t	width=0;
	size_t	height=0;
	GLenum	format = GL_RGBA;
	GLenum	type = GL_UNSIGNED_BYTE;
	switch( loadedTexture )
	{
		case 0: return;
		case 1:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewGray8 );
			internalFormat = 1;
			width  = viewGray8->width();
			height = viewGray8->height();
			format = GL_LUMINANCE;
			type   = GL_UNSIGNED_BYTE;
			break;
		case 2:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgb8 );
			internalFormat = 3;
			width  = viewRgb8->width();
			height = viewRgb8->height();
			format = GL_RGB;
			type   = GL_UNSIGNED_BYTE;
			break;
		case 3:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba8 );
			internalFormat = 4;
			width  = viewRgba8->width();
			height = viewRgba8->height();
			format = GL_RGBA;
			type   = GL_UNSIGNED_BYTE;
			break;
		case 4:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewGray16 );
			internalFormat = 1;
			width  = viewGray16->width();
			height = viewGray16->height();
			format = GL_LUMINANCE;
			type   = GL_UNSIGNED_SHORT;
			break;
		case 5:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgb16 );
			internalFormat = 3;
			width  = viewRgb16->width();
			height = viewRgb16->height();
			format = GL_RGB;
			type   = GL_UNSIGNED_SHORT;
			break;
		case 6:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba16 );
			internalFormat = 4;
			width  = viewRgba16->width();
			height = viewRgba16->height();
			format = GL_RGBA;
			type   = GL_UNSIGNED_SHORT;
			break;
		case 7:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewGray32 );
			internalFormat = 1;
			width  = viewGray32->width();
			height = viewGray32->height();
			format = GL_LUMINANCE;
			type   = GL_FLOAT;
			break;
		case 8:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgb32 );
			internalFormat = 3;
			width  = viewRgb32->width();
			height = viewRgb32->height();
			format = GL_RGB;
			type   = GL_FLOAT;
			break;
		case 9:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba32 );
			internalFormat = 4;
			width  = viewRgba32->width();
			height = viewRgba32->height();
			format = GL_RGBA;
			type   = GL_FLOAT;
			break;
		case 10:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewGray32f );
			internalFormat = 1;
			width  = viewGray32f->width();
			height = viewGray32f->height();
			format = GL_LUMINANCE;
			type   = GL_FLOAT;
			break;
		case 11:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgb32f );
			internalFormat = 3;
			width  = viewRgb32f->width();
			height = viewRgb32f->height();
			format = GL_RGB;
			type   = GL_FLOAT;
			break;
		case 12:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba32f );
			internalFormat = 4;
			width  = viewRgba32f->width();
			height = viewRgba32f->height();
			format = GL_RGBA;
			type   = GL_FLOAT;
			break;
	}

	
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

void loadAlphaTexture( )
{
	unsigned char* dataPtr = NULL;
	
	GLint	internalFormat = 1;
	size_t	width=0;
	size_t	height=0;
	GLenum	format = GL_RGBA;
	GLenum	type = GL_UNSIGNED_BYTE;
	switch( loadedTexture )
	{

		case 3:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba8 );
			internalFormat = 4;
			width  = viewRgba8->width();
			height = viewRgba8->height();
			format = GL_RGBA;
			type   = GL_UNSIGNED_BYTE;
			break;
		case 6:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba16 );
			internalFormat = 4;
			width  = viewRgba16->width();
			height = viewRgba16->height();
			format = GL_RGBA;
			type   = GL_UNSIGNED_SHORT;
			break;

		case 9:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba32 );
			internalFormat = 4;
			width  = viewRgba32->width();
			height = viewRgba32->height();
			format = GL_RGBA;
			type   = GL_FLOAT;
			break;

		case 12:
			dataPtr = (unsigned char*)boost::gil::interleaved_view_get_raw_data( *viewRgba32f );
			internalFormat = 4;
			width  = viewRgba32f->width();
			height = viewRgba32f->height();
			format = GL_RGBA;
			type   = GL_FLOAT;
			break;
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 7:
		case 8:
		case 10:
		case 11:
			return;
	}

	
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, dataPtr );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

void setTransfert( float red, float green, float blue, float alpha=1.f )
{
	switch( loadedTexture )
	{
		case 0 : /// undefined type
		case 1 : /// Gray 8  bits
		case 4 : /// Gray 16 bits
		case 7 : /// Gray 32 bits
		case 10: /// Gray 32 float
			return;
		case 2 : /// RGB  8  bits
		case 5 : /// RGB  16 bits
		case 8 : /// RGB  32 bits
		case 11: /// RGB  32 float
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			break;
		case 3 : /// RGBA 8  bits
		case 6 : /// RGBA 16 bits
		case 9 : /// RGBA 32 bits
		case 12: /// RGBA 32 float
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			glPixelTransferf( GL_ALPHA_SCALE, alpha );
			break;
	}
	reloadTexture( );
}

void showRedChannelTexture( )
{
	if( ! showRedChannel )
	{
		setTransfert( 1.f, 0.f, 0.f );
		showRedChannel = true;
		showGreenChannel = false;
		showBlueChannel = false;
		showAlphaChannel = false;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f );
		showRedChannel = false;
	}
	glutPostRedisplay ();
}

void showGreenChannelTexture( )
{
	if( ! showGreenChannel )
	{
		setTransfert( 0.f, 1.f, 0.f );
		showRedChannel = false;
		showGreenChannel = true;
		showBlueChannel = false;
		showAlphaChannel = false;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f );
		showGreenChannel = false;
	}
	glutPostRedisplay ();
}

void showBlueChannelTexture( )
{
	if( ! showBlueChannel )
	{
		setTransfert( 0.f, 0.f, 1.f );
		showRedChannel = false;
		showGreenChannel = false;
		showBlueChannel = true;
		showAlphaChannel = false;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f );
		showBlueChannel = false;
	}
	glutPostRedisplay ();
}

void showAlphaChannelTexture( )
{
	if( ! showAlphaChannel )
	{
		loadAlphaTexture( );
		showRedChannel = false;
		showGreenChannel = false;
		showBlueChannel = false;
		showAlphaChannel = true;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f, 1.f );
		showAlphaChannel = false;
	}
	glutPostRedisplay ();
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
			
		case 'r':
			showRedChannelTexture( ); 
			break;
		case 'g':
			showGreenChannelTexture( ); 
			break;
		case 'b':
			showBlueChannelTexture( ); 
			break;
		case 'a':
			showAlphaChannelTexture();
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
				if( x < viewGray8->width() && y < viewGray8->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(5) << (int) get_color( viewGray8[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 2 : // rgb 8
				if( x < viewRgb8->width() && y < viewRgb8->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(5) << (int) get_color( viewRgb8[0](x,y), red_t() ) << " " <<
											std::setw(5) << (int) get_color( viewRgb8[0](x,y), green_t() ) << " " <<
											std::setw(5) << (int) get_color( viewRgb8[0](x,y), blue_t() ) << std::endl;
				break;
			case 3 : // rgba 8
				if( x < viewRgba8->width() && y < viewRgba8->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(5) << (int) get_color( viewRgba8[0](x,y), red_t() ) << " " <<
											std::setw(5) << (int) get_color( viewRgba8[0](x,y), green_t() ) << " " <<
											std::setw(5) << (int) get_color( viewRgba8[0](x,y), blue_t() ) << " " <<
											std::setw(5) << (int) get_color( viewRgba8[0](x,y), alpha_t() ) << std::endl;
				break;
			case 4 :
				if( x < viewRgb16->width() && y < viewRgb16->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(7) << get_color( viewGray16[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 5 :
				if( x < viewRgb16->width() && y < viewRgb16->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(7) << get_color( viewRgb16[0](x,y), red_t() ) << " " <<
											std::setw(7) << get_color( viewRgb16[0](x,y), green_t() ) << " " <<
											std::setw(7) << get_color( viewRgb16[0](x,y), blue_t() ) << std::endl;
				break;
			case 6 :
				if( x < viewRgba16->width() && y < viewRgba16->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(7) << get_color( viewRgba16[0](x,y), red_t() ) << " " <<
											std::setw(7) << get_color( viewRgba16[0](x,y), green_t() ) << " " <<
											std::setw(7) << get_color( viewRgba16[0](x,y), blue_t() ) << " " <<
											std::setw(7) << get_color( viewRgba16[0](x,y), alpha_t() ) << std::endl;
				break;
			case 7 :
				if( x < viewGray32->width() && y < viewGray32->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(12) << get_color( viewGray32[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 8 :
				if( x < viewRgb32->width() && y < viewRgb32->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(12) << get_color( viewRgb32[0](x,y), red_t() ) << " " <<
											std::setw(12) << get_color( viewRgb32[0](x,y), green_t() ) << " " <<
											std::setw(12) << get_color( viewRgb32[0](x,y), blue_t() ) << std::endl;
				break;
			case 9 :
				if( x < viewRgba32->width() && y < viewRgba32->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(12) << get_color( viewRgba32[0](x,y), red_t() ) << " " <<
											std::setw(12) << get_color( viewRgba32[0](x,y), green_t() ) << " " <<
											std::setw(12) << get_color( viewRgba32[0](x,y), blue_t() ) << " " <<
											std::setw(12) << get_color( viewRgba32[0](x,y), alpha_t() ) << std::endl;
				break;
			case 10:
				if( x < viewGray32f->width() && y < viewGray32f->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(12) << get_color( viewGray32f[0](x,y), gray_color_t() ) << std::endl;
				break;
			case 11:
				if( x < viewRgb32f->width() && y < viewRgb32f->height() )
					std::cout << "at " <<	std::setw(4) << x << "," <<
											std::setw(4) << y << ": " <<
											std::setw(12) << get_color( viewRgb32f[0](x,y), red_t() ) << " " <<
											std::setw(12) << get_color( viewRgb32f[0](x,y), green_t() ) << " " <<
											std::setw(12) << get_color( viewRgb32f[0](x,y), blue_t() ) << std::endl;
				break;
			case 12:
				if( x < viewRgba32f->width() && y < viewRgba32f->height() )
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
