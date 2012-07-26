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
int   argc    = 1;

// image properties
struct ImgProperties
{
	const char* data;
	GLint  internalFormat;
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
int w_out, h_out;
int angle_cam = 60;
float zoom = 1;

// channel properties
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

void loadNewTexture( const ImgProperties& properties )
{
	// loading texture
	glTexImage2D( GL_TEXTURE_2D, 0, properties.internalFormat, properties.width, properties.height, 0, properties.format, properties.type, properties.data );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_TEXTURE_2D);
}

void loadNewTexture( const char* data, GLint internalFormat, size_t width, size_t height, GLenum format, GLenum type )
{
	img.data = data;
	img.internalFormat = internalFormat;
	img.width = width;
	img.height = height;
	img.format = format;
	img.type = type;

	switch( img.format )
	{
		case GL_LUMINANCE : img.component = 1; break;
		case GL_RGB       : img.component = 3; break;
		case GL_RGBA      : img.component = 4; break;
	}

	loadNewTexture( img );
}


void reloadTexture( )
{
	loadNewTexture( img );
}

void loadAlphaTexture( )
{
	// @TODO find howto pass alpha at luminance
	glTexImage2D( GL_TEXTURE_2D, 0, img.internalFormat, img.width, img.height, 0, img.format, img.type, img.data );
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glEnable(GL_TEXTURE_2D);
}

void setTransfert( float red, float green, float blue, float alpha=1.f )
{
	switch( img.format )
	{
		case GL_LUMINANCE :
			return;
		case GL_RGB :
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			break;
		case GL_RGBA :
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			glPixelTransferf( GL_ALPHA_SCALE, alpha );
			break;
	}
}

void displayChannelTexture( bool& channel, const float red, const float green, const float blue )
{
	ImgProperties p = img;
	if( ! channel )
	{
		setTransfert( red, green, blue );
		showRedChannel   = false;
		showGreenChannel = false;
		showBlueChannel  = false;
		showAlphaChannel = false;
		channel = true;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f );
		channel = false;
	}
	loadNewTexture( p );

	glutPostRedisplay();
}

void showRedChannelTexture( )
{
	displayChannelTexture( showRedChannel, 1.f, 0.f, 0.f );
}

void showGreenChannelTexture( )
{
	displayChannelTexture( showGreenChannel, 0.f, 1.f, 0.f );
}

void showBlueChannelTexture( )
{
	displayChannelTexture( showBlueChannel, 0.f, 0.f, 1.f );
}

void showAlphaChannelTexture( )
{
	if( ! showAlphaChannel )
	{
		loadAlphaTexture( );
		showRedChannel   = false;
		showGreenChannel = false;
		showBlueChannel  = false;
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
	glVertex2f  ( -1, 1 );
	
	glTexCoord2f( 0, 1 );
	glVertex2f  ( -1, -1 );
	
	glTexCoord2f( 1, 1 );
	glVertex2f  ( 1, -1 );
	
	glTexCoord2f( 1, 0 );
	glVertex2f  ( 1, 1 );
	
	glEnd();
	glutSwapBuffers();
}

void idle()
{
}

void displayInformations()
{
	std::string textureType;
	switch( img.format )
	{
		case GL_LUMINANCE : textureType = "Gray "; break;
		case GL_RGB       : textureType = "RGB  "; break;
		case GL_RGBA      : textureType = "RGBA "; break;
	}
	switch( img.type )
	{
		case GL_UNSIGNED_BYTE  : textureType += "8 bits"; break;
		case GL_UNSIGNED_SHORT : textureType += "16 bits"; break;
		case GL_FLOAT          : textureType += "32 float"; break;
	}
	std::cout << textureType << " " << w_out << "x" << h_out << std::endl;
}

void displayHelp()
{
	std::cout << kViewerHelp << std::endl;
}

void keyboard(unsigned char k, int x, int y)
{
	switch( k )
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
		std::cout << "at " <<	std::setw(4) << x << "," << std::setw(4) << y << ": ";

		for( size_t i = 0; i < img.component; i++ )
		{
			size_t idx = ( x + y* img.width ) * img.component + i;
			switch( img.type )
			{
				case GL_UNSIGNED_BYTE:
				{
					const unsigned char* d = (const unsigned char*) img.data;
					std::cout << std::setw(5) << (unsigned int) d[idx] ;
					break;
				}
				case GL_UNSIGNED_SHORT:
				{
					const unsigned short* d = (const unsigned short*) img.data;
					std::cout << std::setw(7) << d[idx] ;
					break;
				}
				case GL_FLOAT:
				{
					const float* d = (const float*) img.data;
					std::cout << std::setw(10) << d[idx] ;
					break;
				}
			}
		}
		std::cout << std::endl;
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
	
	glutReshapeFunc(reshape);
}



}
}
}

#endif
