#include <GL/glew.h> // need to be included before gl.h

#include "CloudPointData.hpp"
#include <tuttle/plugin/opengl/gl.h>

#include <boost/gil/extension/io/png_io.hpp>


namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
CloudPointData::CloudPointData(const OfxPointI& size, OfxTime time)
{
	_size = size;	//define first size (current clip number of pixels)
	_time = time;	//get current time
	
	int imgSize = size.x*size.y; // number of pixel in the image
	_imgCopy.reserve( imgSize * 0.5 );	//reserve memory for buffer
	_averageColor.x = _averageColor.y = _averageColor.z = 0; //initialize average
	
	//create geodesic form
	Ofx3DPointD p = {0,0,0};
	_geodesicForm.subdiviseFaces(p,4);
}

/**
 * Open src clip return if opening has been done (y or n)
 * @param clipSrc	source of the plugin
 * @param renderScale	current renderScale
 */
bool CloudPointData::generateVBOData( OFX::Clip* clipSrc, const OfxPointD& renderScale, const bool vboWithDiscretization, const int discretizationStep )
{	
	// connection test
	if( ! clipSrc->isConnected() )
	{	
		return false;
	}
	
	boost::scoped_ptr<OFX::Image> src( clipSrc->fetchImage(_time, clipSrc->getCanonicalRod(_time)) );	//scoped pointer of current source clip
	
	//TUTTLE_TCOUT_VAR( clipSrc->getPixelRod(_time,renderScale)); 
	//TUTTLE_TCOUT_VAR(clipSrc->getCanonicalRod(_time, renderScale));
	
	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		std::cout << "src is not accessible" << std::endl;
		return false;
	}
	//TUTTLE_TCOUT_VAR( src->getRowBytes());
	if( src->getRowBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		return false;
	}
	const OfxRectI srcPixelRod = clipSrc->getPixelRod( _time, renderScale ); //get current RoD
	if( (clipSrc->getPixelDepth() != OFX::eBitDepthFloat) ||
		(clipSrc->getPixelComponents() == OFX::ePixelComponentNone) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return false;
	}
	
	//TUTTLE_TCOUT_VAR( src->getBounds());
	//TUTTLE_TCOUT_VAR( src->getRegionOfDefinition() );
	
	if( srcPixelRod != src->getBounds() )// the host does bad things !
	{
		// remove overlay... but do not crash.
		TUTTLE_COUT_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return false;
	}
	
	// Compute if source is OK
	SView srcView = tuttle::plugin::getView<SView>( src.get(), srcPixelRod );	// get current view from source clip
	//TUTTLE_TCOUT_VAR(srcView.width());
	//TUTTLE_TCOUT_VAR(srcView.height());
	//TUTTLE_COUT_INFOS;
	
//	TUTTLE_TCOUT_VAR( boost::gil::get_color( average, boost::gil::red_t() ) );
//	TUTTLE_TCOUT_VAR( boost::gil::get_color( average, boost::gil::green_t() ) );
//	TUTTLE_TCOUT_VAR( boost::gil::get_color( average, boost::gil::blue_t() ) );
	
	_imgCopy.clear();		//clear buffer
	//TUTTLE_COUT_INFOS;
	if( vboWithDiscretization )	//does user want to discretize the VBO
	{
		//TUTTLE_COUT_INFOS;
		generateDiscretizedVBOData( srcView, discretizationStep ); //create data and return buffer size
	}
	else
	{
		//TUTTLE_COUT_INFOS;
		generateAllPointsVBOData( srcView ); // create data and return buffer size
	}	
	
	//TUTTLE_COUT_INFOS;
	return true;
}

/**
 * create the VBO from VBO data (draw function)
 */
void CloudPointData::updateVBO()
{
	//TUTTLE_TCOUT_INFOS;
	_imgVBO.createVBO( &(_imgCopy.front()), _imgCopy.size() / 3 ); //generate VBO to draw
	//TUTTLE_COUT_INFOS;
	_imgVBO._color = true;	//activate color for VBO
}

/**
 * Copy rgb channels of the clip source into a buffer
 */
int CloudPointData::generateAllPointsVBOData(SView srcView)
{
	//compute buffer size
	int size = (int)(srcView.height()*srcView.width());	//return size : full img here
	
	//TUTTLE_TCOUT_INFOS;
	//copy full image into buffer
	Pixel_copy funct( _imgCopy );	//functor declaration	
	
	//TUTTLE_TCOUT_INFOS;
	boost::gil::transform_pixels( srcView, funct );		//with functor reference
	//TUTTLE_TCOUT_INFOS;

	return size;
}

/**
 * Copy discretized rgb channels of the clip source into a buffer
 */
int CloudPointData::generateDiscretizedVBOData(SView srcView, int discretizationStep)
{
	//compute buffer size
	int size = (int)(srcView.height()*srcView.width());	//return size : full img here

	//Create and use functor to get discretize data  (functor with template)
	Pixel_copy_discretization<SPixel> funct(_imgCopy,discretizationStep);	//functor declaration	
	boost::gil::transform_pixels( srcView, funct);							//with functor reference
	funct.convertSetDataToVectorData();								//deplace functor data to _imgCopy data
	return size;
}

/*
 *Generate average from color selection clip
 */
bool CloudPointData::generateAverageColorSelection(OFX::Clip* clipColor, const OfxPointD& renderScale)
{
	// connection test
	if( ! clipColor->isConnected() )
	{	
		return false;
	}

	boost::scoped_ptr<OFX::Image> src( clipColor->fetchImage(_time, clipColor->getCanonicalRod(_time)) );	//scoped pointer of current source clip
	
	//TUTTLE_TCOUT_VAR( clipColor->getPixelRod(_time,renderScale)); 
	//TUTTLE_TCOUT_VAR( clipColor->getCanonicalRod(_time, renderScale));

	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		std::cout << "color src is not accessible" << std::endl;
		return false;
	}

	if( src->getRowBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		return false;
	}

	const OfxRectI srcPixelRod = clipColor->getPixelRod( _time, renderScale ); //get current RoD
	if( (clipColor->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipColor->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return false;
	}

	//TUTTLE_TCOUT_VAR( src->getBounds());
	//TUTTLE_TCOUT_VAR( src->getRegionOfDefinition() );

	if( srcPixelRod != src->getBounds() )// the host does bad things !
	{
		// remove overlay... but do not crash.
		TUTTLE_COUT_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return false;
	}

	// Compute if source is OK
	SView colorView = tuttle::plugin::getView<SView>( src.get(), srcPixelRod );		// get current view from color clip
	ComputeAverage<SView>::CPixel average = ComputeAverage<SView>()( colorView );	// compute color clip average
	
	//copy computed average into average stock variable
	_averageColor.x = average[0]; //red channel value
	_averageColor.y = average[1]; //green channel value
	_averageColor.z = average[2]; //blue channel values
	
	return true; //average has been computed
}

/**
 * Draw average on screen (cross)
 */
void CloudPointData::drawAverage()
{
	float kCrossSize = 0.05f;
	//compute complementary color
	float complementaryColor[3]; 
	complementaryColor[0] = 1-_averageColor.x;	//complementary red
	complementaryColor[1] = 1-_averageColor.y;	//complementary green
	complementaryColor[2] = 1-_averageColor.z;	//complementary blue
	
	//compute values on X axis
	float xBefore = _averageColor.x-kCrossSize;	//compute before value (X axis)
	float xAfter = _averageColor.x + kCrossSize;	//compute after value (X axis)
	
	//compute values on Y axis
	float yBefore = _averageColor.y-kCrossSize;	//compute before value (Y axis)
	float yAfter = _averageColor.y + kCrossSize;	//compute after value (Y axis)
	
	//compute values on Z axis
	float zBefore = _averageColor.z-kCrossSize;	//compute before value (Z axis)
	float zAfter = _averageColor.z+kCrossSize;	//compute after value (Z axis)
	
	//drawing average mark
	glBegin(GL_LINES);
	glColor3f(complementaryColor[0],complementaryColor[1],complementaryColor[2]); //color : complementary to average
	glVertex3f(xBefore,_averageColor.y,_averageColor.z); glVertex3f(xAfter,_averageColor.y,_averageColor.z); //X axis
	glVertex3f(_averageColor.x,yBefore,_averageColor.z); glVertex3f(_averageColor.x,yAfter,_averageColor.z); //Y axis
	glVertex3f(_averageColor.x,_averageColor.y,zBefore); glVertex3f(_averageColor.x,_averageColor.y,zAfter); //Z axis
	glEnd();
}


////////////////////////////////////////////////////////////////////////////////
//                           CloudPoint::VBO                                   //
////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * @param data	data used to create VBO
 * @param size	size of VBO	
 * @param usage	usage of VBO
 */
void CloudPointData::VBO::createVBO( const void* data, int size, GLenum usage )
{
	//TUTTLE_TCOUT( "id creation : " << _id <<"  size : "<< size <<"  usage : "<< usage );
	_size = size;
	//TUTTLE_TCOUT_INFOS;
	genBuffer( _id, data, size, GL_ARRAY_BUFFER, usage );
	//
	//TUTTLE_TCOUT_INFOS;
}

/**
 * destroy a VBO
 * If VBO id is not valid or zero, then OpenGL ignores it silently.
 */
void CloudPointData::VBO::deleteVBO( )
{
	//std::cout << "id destruction : " << _id << std::endl;
	if( _id != 0 )	//if VBO exists
	{
		glDeleteBuffers( 1, &_id );
		_id = 0; // 0 is reserved, glGenBuffersARB() will return non-zero id if success
	}
}

/**
 * generate vertex buffer object and bind it with its data
 * You must give 2 hints about data usage; target and mode, so that OpenGL can
 * decide which data should be stored and its location.
 * VBO works with 2 different targets; GL_ARRAY_BUFFER for vertex arrays
 * and GL_ELEMENT_ARRAY_BUFFER for index array in glDrawElements().
 * The default target is GL_ARRAY_BUFFER.
 * By default, usage mode is set as GL_STATIC_DRAW.
 * Other usages are GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
 * GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
 * GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY.
 */
void CloudPointData::VBO::genBuffer( unsigned int& id, const void* data, int size, GLenum target, GLenum usage )
{
	//TUTTLE_TCOUT_INFOS;
	if( id != 0 )
		deleteVBO( );

	const int dataSize = size * 3 * sizeof(float);

	//TUTTLE_TCOUT_INFOS;
	glGenBuffers( 1, &(_id) );		// create a vbo
	//TUTTLE_TCOUT_INFOS;
	glBindBuffer( target, _id );	// activate vbo id to use
	//TUTTLE_TCOUT_INFOS;
	glBufferData( target, dataSize, data, usage ); // upload data to video card
	//TUTTLE_TCOUT_INFOS;
	
	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameteriv( target, GL_BUFFER_SIZE, &bufferSize );
	//TUTTLE_TCOUT_INFOS;
	if( dataSize != bufferSize )
	{
		deleteVBO( );
		std::cout << "[createVBO()] Mismatch between Data size and input array" << std::endl;
	}
	//TUTTLE_TCOUT_INFOS;
}

/**
 * Draw the current VBO on screen
 */
void CloudPointData::VBO::draw( )
{
	if( _id && _size )
	{
		// bind VBOs with IDs and set the buffer offsets of the bound VBOs
		// When buffer object is bound with its ID, all pointers in gl*Pointer()
		// are treated as offset instead of real pointer.
		glBindBuffer( GL_ARRAY_BUFFER, _id );
		glVertexPointer( 3, GL_FLOAT, 0, 0 );
		glEnableClientState( GL_VERTEX_ARRAY ); // enable vertex arrays

		if( _color )
		{
			glColorPointer( 3, GL_FLOAT, 0, 0 ); //same buffer is used for colors
			glEnableClientState( GL_COLOR_ARRAY );
		}

		glDrawArrays( GL_POINTS, 0, _size );

		glDisableClientState( GL_VERTEX_ARRAY ); // disable vertex arrays

		if( _color )
			glDisableClientState( GL_COLOR_ARRAY );


		// it is good idea to release VBOs with ID 0 after use.
		// Once bound with 0, all pointers in gl*Pointer() behave as real
		// pointer, so, normal vertex array operations are re-activated
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
}


}
}
}