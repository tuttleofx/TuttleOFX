#include "DiffPlotDisplay.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <boost/gil/extension/color/hsv.hpp>
#include <boost/foreach.hpp>


namespace tuttle {
namespace plugin {
namespace diffPlot {

void displayGrid(float height, float width)
{
	glBegin(GL_LINES);
	//draw standard reference
	glColor3f(.9f,.9f,.9f);								//white color
	glVertex2f(-0.1f,0); glVertex2f(-0.1f,height);		//Y axis
	glVertex2f(0,-0.1f); glVertex2f(width,-0.1f);		//X axis
	
	//compute step X and Y
	float stepY = (float)(height/10.0f);
	float stepX = (float)(width/10.0f);
	float stepYm = (float)(height/20.0f);
	float stepXm = (float)(width/20.0f);
	
	//drawing minor grid
	glColor3f(.2f,.2f,.2f);		//gray color
	float baseY = 0;			//initialize y to O
	float baseX = 0;			//initialize x to 0
	for(unsigned int i=0; i< 20; ++i)
	{
		baseY += stepYm;	//update Y position
		baseX += stepXm; //update X position
		glVertex2f(-0.1f,baseY);	glVertex2f(width,baseY); //draw Y axis
		glVertex2f(baseX, -0.1f);	glVertex2f(baseX,height);//draw X axis
	}
	
	//drawing major grid
	glColor3f(.5f,.5f,.5f);		//gray color
	baseY = 0;			//initialize y to O
	baseX = 0;			//initialize x to 0
	for(unsigned int i=0; i< 10; ++i)
	{
		baseY += stepY;	//update Y position
		baseX += stepX; //update X position
		glVertex2f(-0.1f,baseY);	glVertex2f(width,baseY); //draw Y axis
		glVertex2f(baseX, -0.1f);	glVertex2f(baseX,height);//draw X axis
	}

	//draw grid
	glEnd();
}

/**
 * @param 
 */
void plotChannelMapping( const DiffPlotMap& data, const DiffPlotMap& selectionData, const int step, const int height, const int width, const double selectionMultiplier )
{
	glBegin( GL_POINTS );
	glColor3f(1., 0., 0.);

	BOOST_FOREACH( const typename DiffPlotMap::value_type& a, data )
	{
		BOOST_FOREACH( const Number b, a.second )
		{
			glVertex2f( a.first, b );
		}
	}
	
	glEnd();
}

//void plotCorrespondances(const DiffPlotVector& selection_v, const double step, const double width, const DiffPlotColor color)
//{
//	glBegin( GL_POINTS );
//	double base_step = 0.0;
//	glColor3f(color._colorBorder.r, color._colorBorder.g, color._colorBorder.b);
//	for(unsigned int i=0; i<selection_v.size(); ++i)
//	{
//		if(selection_v.at(i) != 0)
//		{
//			glVertex2f((float)(base_step), -10.0f);
//		}	
//		base_step += step;
//	}
//	glEnd();
//}

/**
 * display the horizontal indicator (openGL)
 * @param size size of the source clip
 */
void displayHorizontalIndicator(const OfxPointI size)
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

}
}
}
