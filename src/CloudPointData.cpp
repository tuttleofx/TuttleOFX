#include <GL/glew.h> // need to be included before gl.h

#include "CloudPointData.hpp"
#include <tuttle/plugin/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
CloudPointData::CloudPointData(const OfxPointI& size)
{
	_size = size;	//define first size (current clip number of pixels)
}

/**
 * Open src clip return if opening has been done (y or n)
 * @param clipSrc	source of the plugin
 * @param time	current time
 * @param renderScale	current renderScale
 */
bool CloudPointData::generateVBO(OFX::Clip* clipSrc, const OfxTime time, const OfxPointD& renderScale, bool vboWithDiscretization, int discretizationStep)
{	
	// connection test
	if( ! clipSrc->isConnected() )
	{	
		return false;
	}
	
	boost::scoped_ptr<OFX::Image> src( clipSrc->fetchImage(time, clipSrc->getCanonicalRod(time)) );	//scoped pointer of current source clip
	
	// Compatibility tests
	if( !src.get() ) // source isn't accessible
	{
		std::cout << "src is not accessible" << std::endl;
		return false;
	}

	if( src->getRowBytes() == 0 )//if source is wrong
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		return false;
	}
	OfxRectI srcPixelRod = clipSrc->getPixelRod( time, renderScale ); //get current RoD
	if( (clipSrc->getPixelDepth() != OFX::eBitDepthFloat) ||
		(!clipSrc->getPixelComponents()) )
	{
		BOOST_THROW_EXCEPTION( exception::Unsupported()	<< exception::user() + "Can't compute histogram data with the actual input clip format." );
        return false;
	}
	
	if( srcPixelRod != src->getBounds() )// the host does bad things !
	{
		// remove overlay... but do not crash.
		TUTTLE_COUT_WARNING( "Image RoD and image bounds are not the same (rod=" << srcPixelRod << " , bounds:" << src->getBounds() << ")." );
		return false;
	}
	
	// Compute if source is OK
	SView srcView = tuttle::plugin::getView<SView>( src.get(), srcPixelRod );	// get current view from source clip
	
	int sizeVBO;	//vbo size
	if(vboWithDiscretization)	//does user want to discretize the VBO
		sizeVBO = generateDiscretizedVBOData(srcView,discretizationStep); //create data and return buffer size
	else
		sizeVBO = generateAllPointsVBOData(srcView); // create data and return buffer size
	
	
	_imgVBO.createVBO(&(_imgCopy.front()),sizeVBO); //generate VBO to draw
	_imgVBO._color = true;	//activate color for VBO
	
	return true;
}


/**
 * Copy rgb channels of the clip source into a buffer
 */
int CloudPointData::generateAllPointsVBOData(SView srcView)
{
	//compute buffer size
	int size = (int)(srcView.height()*srcView.width());	//return size : full img here
	
	//clear data
	_imgCopy.clear();		//clear buffer
	_imgVBO.deleteVBO();	// delete previous VBO
	//copy full image into buffer
	Pixel_copy funct(_imgCopy);	//functor declaration	
	boost::gil::transform_pixels( srcView, funct);		//with functor reference
	return size;
}

/**
 * Copy discretized rgb channels of the clip source into a buffer
 */
int CloudPointData::generateDiscretizedVBOData(SView srcView, int discretizationStep)
{
	//compute buffer size
	int size = (int)(srcView.height()*srcView.width());	//return size : full img here
	
	//clear data
	_imgCopy.clear();		//clear buffer
	_imgVBO.deleteVBO();	// delete previous VBO

	//copy full image into buffer
	Pixel_copy_discretization funct(_imgCopy,discretizationStep);	//functor declaration	
	boost::gil::transform_pixels( srcView, funct);					//with functor reference
	return size;
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
	_size = size;
	genBuffer( _id, data, size, GL_ARRAY_BUFFER, usage );
}

/**
 * destroy a VBO
 * If VBO id is not valid or zero, then OpenGL ignores it silently.
 */
void CloudPointData::VBO::deleteVBO( )
{
	glDeleteBuffers( 1, &_id );
	_id = 0; // 0 is reserved, glGenBuffersARB() will return non-zero id if success
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
	if( id != 0 )
		deleteVBO( );

	int dataSize = size * 3 * sizeof(float);

	glGenBuffers( 1, &(_id) );		// create a vbo
	glBindBuffer( target, _id );	// activate vbo id to use
	glBufferData( target, dataSize, data, usage ); // upload data to video card
	
	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameteriv( target, GL_BUFFER_SIZE, &bufferSize );
	if( dataSize != bufferSize )
	{
		deleteVBO( );
		std::cout << "[createVBO()] Mismatch between Data size and input array" << std::endl;
	}
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