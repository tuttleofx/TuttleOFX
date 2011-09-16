#ifndef _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_VIEWER_ALGORITHM_HPP_

//#include <boost/gil/extension/channel.hpp>

#ifdef _WINDOWS_
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

namespace tuttle {
namespace plugin {
namespace viewer {

GLuint LoadTextureFormABuffer( const BYTE* data, const size_t& width, const size_t& height, const size_t& channels )
{
	int wrap =0;
	GLuint texture;

	// allocate a texture name
	glGenTextures( 1, &texture );

	// select our current texture
	glBindTexture( GL_TEXTURE_2D, texture );

	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// when texture area is small, bilinear filter the closest MIP map
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		   GL_LINEAR_MIPMAP_NEAREST );
	// when texture area is large, bilinear filter the first MIP map
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// if wrap is true, the texture wraps over at the edges (repeat)
	//       ... false, the texture ends at the edges (clamp)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

	// build our texture MIP maps
	gluBuild2DMipmaps( GL_TEXTURE_2D, channels, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );

	return texture;

}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}

// Window Proc ///////////////////////////////////////////////////////

LRESULT CALLBACK WndProc( HWND hWnd, UINT message,
			  WPARAM wParam, LPARAM lParam )
{

  switch ( message ) {

  case WM_CREATE:
    return 0;

  case WM_CLOSE:
    PostQuitMessage( 0 );
    return 0;

  case WM_DESTROY:
    return 0;

  case WM_KEYDOWN:
    switch ( wParam ) {

    case VK_ESCAPE:
      PostQuitMessage( 0 );
      return 0;

    }
    return 0;

  default:
    return DefWindowProc( hWnd,
      message, wParam, lParam );

  }

}

// OpenGL ////////////////////////////////////////////////////////////

// Enable OpenGL

void EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC )
{
  PIXELFORMATDESCRIPTOR pfd;
  int iFormat;

  // get the device context (DC)
  *hDC = GetDC( hWnd );

  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize = sizeof( pfd );
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;
  iFormat = ChoosePixelFormat( *hDC, &pfd );
  SetPixelFormat( *hDC, iFormat, &pfd );

  // create and enable the render context (RC)
  *hRC = wglCreateContext( *hDC );
  wglMakeCurrent( *hDC, *hRC );

}

// Disable OpenGL

void DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC )
{
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( hRC );
  ReleaseDC( hWnd, hDC );
}

/*

  // WinMain ///////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
		    LPSTR lpCmdLine, int iCmdShow )
{
  WNDCLASS wc;
  HWND hWnd;
  HDC hDC;
  HGLRC hRC;
  MSG msg;
  BOOL bQuit = FALSE;

  GLuint texture;
  float theta = 0.0f;

  // register window class
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "GLSample";
  RegisterClass( &wc );

  // create main window
  hWnd = CreateWindow(
    "GLSample", "OpenGL Texture Sample",
    WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
    0, 0, 256, 256,
    NULL, NULL, hInstance, NULL );

  // enable OpenGL for the window
  EnableOpenGL( hWnd, &hDC, &hRC );

  // load our texture
  texture = LoadTextureRAW( "texture.raw", TRUE );

  // program main loop
  while ( !bQuit ) {

    // check for messages
    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {

      // handle or dispatch messages
      if ( msg.message == WM_QUIT ) {
	bQuit = TRUE;
      } else {
	TranslateMessage( &msg );
	DispatchMessage( &msg );
      }

    } else {

      // OpenGL animation code goes here

      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT );

      // setup texture mapping
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, texture );

      glPushMatrix();
      glRotatef( theta, 0.0f, 0.0f, 1.0f );
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
      glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
      glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
      glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
      glEnd();
      glPopMatrix();

      SwapBuffers( hDC );

      theta += 1.0f;

    }

  }

  // free the texture
  FreeTexture( texture );

  // shutdown OpenGL
  DisableOpenGL( hWnd, hDC, hRC );

  // destroy the window explicitly
  DestroyWindow( hWnd );

  return msg.wParam;

}

*/


}
}
}

#endif
