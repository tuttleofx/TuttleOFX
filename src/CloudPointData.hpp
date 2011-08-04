#ifndef CLOUDPOINTDATA_HPP
#define	CLOUDPOINTDATA_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include "GeodesicForm.hpp"
#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>

#include<vector>
#include<set>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

typedef std::vector<float, OfxAllocator<float> > DataVector;
typedef boost::gil::rgba32f_view_t SView;
typedef boost::gil::rgba32f_pixel_t SPixel;
	

template <typename PixelRef1, // models pixel concept
          typename PixelRef2> // models pixel concept
struct pixel_is_lesser_t
{
	GIL_FORCEINLINE
    bool operator()( const PixelRef1& p1,
                     const PixelRef2& p2) const
	{
		for( int v = 0; v < 3/*boost::gil::num_channels<Pixel>::type::value*/; ++v )
		{
			if( p1[v] < p2[v] )
				return true;
			if( p1[v] > p2[v] )
				return false;
		}
        return false;
    }
};

//Functor : copy image into a buffer (ignore alpha channel)
struct Pixel_copy
{
	//Arguments
    DataVector& _data; //buffer to fill up
	
	//Constructor
	Pixel_copy(DataVector& data)
	:_data( data )
	{
	}
	//Operator ()
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;	
		//RGB
		for( int v = 0; v < 3 /*boost::gil::num_channels<Pixel>::type::value*/; ++v )
		{
			const float val = boost::gil::channel_convert<boost::gil::bits32f>( p[v] );		//capt channel (red, green or blue)
			_data.push_back( val );	//add value to buffer data
		}
        return p;
    }
};

//Functor copy image (with discretization)
template< typename Pixel>
struct Pixel_copy_discretization
{
	typedef std::set< SPixel, pixel_is_lesser_t<SPixel,SPixel> > PixelSet;
	typedef std::set<SPixel>::iterator PixelSetIterator;
	//Arguments
    DataVector& _data; //buffer to fill up

	int _nbStep;	   // discretization step
	float _step;	   // discretization value
	PixelSet _setData; //we use to set to prevent double same data many times
			
	//Constructor
	Pixel_copy_discretization(DataVector& data, int nbStep):
	_data( data ),
	_nbStep(nbStep)
	{
		_step = (float)(1/(float)(nbStep-1));	//compute discretization value
	}
	
	//Operator ()	
    Pixel operator()( const Pixel& p )
    {
		Pixel add;		//used to put data in the std::set
        using namespace boost::gil;	
		for( int v = 0; v < 3 /*boost::gil::num_channels<Pixel>::type::value*/; ++v ) //We don't want work with alpha channel
		{
			double val = p[v];		//capt channel (red, green or blue)
			
			bool placedInVector = false;	//values is not in the vector yet
			int iteration = 0;				//initialize index to 0
			while(!placedInVector)			//place round value in the vector
			{
				float previousValue = iteration*_step;
				float nextValue = (iteration+1)*_step;
				if(val <= previousValue)
				{
					add[v] = previousValue;				//add value to pixel data
					placedInVector = true;				// value is put in the vector
				}
				else if( val < nextValue)
				{
					add[v] = nextValue;					//add value to pixel data
					placedInVector = true;				//value is put in the vector
				}
				++iteration;
			}
		}
		_setData.insert(add);
		return p;
    }
	
	//Convert _setData values to _data values
	void convertSetDataToVectorData()
	{
		//iterator creation
		for(PixelSetIterator it=_setData.begin(); it != _setData.end(); it++ )	//iterator used to scan the set
		{
			 Pixel p = *it;	//get set current data
			 _data.push_back(p[0]);	//red channel copy into vector
			 _data.push_back(p[1]);	//green channel copy into vector
			 _data.push_back(p[2]);	//blue channel copy into vector
		}
	}
};

//Functor used to compute average on color clip selection
template<class View, typename CType = boost::gil::bits64f>
struct ComputeAverage
{
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::color_space_type<View>::type Colorspace;
	typedef boost::gil::pixel<CType, boost::gil::layout<Colorspace> > CPixel; // the pixel type use for computation (using input colorspace)

	CPixel operator()( const View& image )
	{
		/*using namespace boost::gil;
		const std::size_t nbPixels = image.width() * image.height();
		CPixel sum;
		pixel_zeros_t<CPixel>( )( sum );

		for( int y = 0;
		     y < image.height();
		     ++y )
		{
			typename View::x_iterator src_it = image.x_at( 0, y );
			for( int x = 0;
			     x < image.width();
			     ++x, ++src_it )
			{
				CPixel pix;
				pixel_assigns_t<Pixel, CPixel>( )( * src_it, pix ); // pix = src_it;
				pixel_plus_assign_t<CPixel, CPixel>( )( pix, sum ); // sum += pix;
			}
		}
		return pixel_divides_scalar_t<CPixel, double>() ( sum, nbPixels );*/
		
		using namespace boost::gil;
		std::size_t nbPixels = 0; 
		CPixel sum; //sum of each pixel
		pixel_zeros_t<CPixel>( )( sum ); //set sum to 0

		for( int y = 0;					//for each lines
		     y < image.height();
		     ++y )
		{
			typename View::x_iterator src_it = image.x_at( 0, y );
			for( int x = 0;
			     x < image.width();
			     ++x, ++src_it )		// for each pixels in a line
			{
				CPixel pix;	//initialize a container pixel
				pixel_assigns_t<Pixel, CPixel>( )( * src_it, pix ); // pix = src_it;
				
				if(pix[3] == 1)	//if current pixel is selected (alpha channel == 1)
				{
					pixel_plus_assign_t<CPixel, CPixel>( )( pix, sum ); // sum += pix;
					++nbPixels; //increments number of selected pixel
				}
			}
		}
		if(nbPixels != 0) // if any selected pixel has been added
			sum = pixel_divides_scalar_t<CPixel, double>() ( sum, nbPixels ); //compute average average (sum/nbPixels)
		return 	sum; //return average (or (0,0,0) pixel)
	}

};


class CloudPointData {

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
	OfxTime _time;				//current time in sequence
	VBO _imgVBO;				//VBO to display on overlay
	DataVector _imgCopy;//copy of the image needed to draw Vector
	//SPixel _selectionColorAverage; //Average of the selection color clip (selection)
	Ofx3DPointD _averageColor;	   //Average of the selection color clip (selection)
	
	GeodesicForm _geodesicForm; //geodesic form

public:
	//Constructor
	CloudPointData(const OfxPointI& size, OfxTime time);
	
	//VBO managment
	bool generateVBOData(OFX::Clip* clipSrc, const OfxPointD& renderScale, bool vboWithDiscretization, int discretizationStep);	//create new VBO data (fill up buffer)
	void updateVBO();	//create the VBO from VBO data (draw function)
	
	//Average management
	bool generateAverageColorSelection(OFX::Clip* clipColor, const OfxPointD& renderScale);	//compute selection color clip average
	void drawAverage();																		//draw average on screen (cross)
	
private:
	//VBO data management
	int generateAllPointsVBOData(SView srcView);	//generate a VBO with all of the pixels
	int generateDiscretizedVBOData(SView srcView, int discretizationStep); //generate a  VBO with discretization
	
	//Average data computing
	void computeColorAverage(SView srcView);	//compute the average of the selection color clip
};

}
}
}
#endif	/* CLOUDPOINTDATA_HPP */

