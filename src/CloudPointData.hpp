#ifndef CLOUDPOINTDATA_HPP
#define	CLOUDPOINTDATA_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include<vector>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

typedef std::vector<float, OfxAllocator<float> > DataVector;
	
struct Pixel_copy
{
	//Arguments
    DataVector& _data; //buffer to fill up
	//Constructor
	Pixel_copy(DataVector& data)
	: _data( data )
	{

	}
	//Operator ()
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;	
		//RGB
		for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value-1; ++v )
		{
			double val = p[v];		//capt channel (red, green or blue)
			if(val < 0)
				val = 0;
			if(val > 1.0)
				val = 1.0;
			_data.push_back((float)val);	//add value to buffer data
		}
        return p;
    }
};

struct Pixel_copy_discretization
{
	//Arguments
    DataVector& _data; //buffer to fill up
	int _nbStep;	   // discretization step
	float _step;	   // discretization value
	int test;	///@todo:remove
	//Constructor
	Pixel_copy_discretization(DataVector& data, int nbStep):
	_data( data ),
	_nbStep(nbStep)
	{
		_step = (float)(1/(float)nbStep);	//compute discretization value
		test=0;
	}
	//Operator ()
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;	
		for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value-1; ++v )
		{
			double val = p[v];		//capt channel (red, green or blue)
			if(val < 0)				//clamp values < 0 to 0
				val = 0;
			if(val > 1.0)			//clamp values > 1 to 1
				val = 1.0;
			
			bool placedInVector = false;	//values is not in the vector yet
			int iteration = 0;				//initialize index to 0
			while(!placedInVector && iteration < _nbStep)			//place round value in the vector
			{
				float previousValue = iteration*_step;
				float nextValue = (iteration+1)*_step;
				if(val <= previousValue)
				{
					_data.push_back(previousValue);		//add value to buffer data
					placedInVector = true;				// value is put in the vector
				}
				else if( val < nextValue)
				{
					_data.push_back(nextValue);			//add value to buffer data
					placedInVector = true;				//value is put in the vector
				}
				++iteration;
			}
		}
		return p;
    }
};

class CloudPointData {
	typedef boost::gil::rgba32f_view_t SView;
protected:
	//VBO class (used to add an OpenGL VBO to scene)
	class VBO
	{
	public:
		//VBO class (for create / remove / draw VBOs)
		VBO( const void* data, int dataSize, GLenum usage ) : _id( 0 ), _size( 0 ), _color( 0 )
		{
			createVBO( data, dataSize, usage );
		}

		VBO( ) : _id( 0 ), _size( 0 ), _color( 0 ){ }

		~VBO( )
		{
			deleteVBO( );
		}

	public:
		unsigned int _id;
		unsigned int _size;
		bool _color;

		void selfColor( bool c )
		{
			_color = c;
		}
		// VBO managment
		void createVBO( const void* data, int size, GLenum usage = GL_STATIC_DRAW );	//create a new VBO
		void deleteVBO( );																//delete the VBO
		void genBuffer( unsigned int& id, const void* data, int size, GLenum target, GLenum usage );//generate VBO data
		void draw( );	//draw VBO on screen
	};
	
public:
	/*Class arguments*/
	OfxPointI _size;			//size of source clip
	VBO _imgVBO;				//VBO to display on overlay
	DataVector _imgCopy;//copy of the image needed to draw Vector
public:
	/*Constructor*/
	CloudPointData(const OfxPointI& size);
	
	/*VBO managment*/
	bool generateVBO(OFX::Clip* clipSrc, const OfxTime time, const OfxPointD& renderScale, bool vboWithDiscretization, int discretizationStep);	//create new VBO from size
	int generateAllPointsVBOData(SView srcView);	//generate a VBO with all of the pixels
	int generateDiscretizedVBOData(SView srcView, int discretizationStep); //generate a  VBO with discretization

};

}
}
}
#endif	/* CLOUDPOINTDATA_HPP */

