#include <GL/glew.h> // need to be included before gl.h

#include "CloudPointData.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <terry/algorithm/transform_pixels.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCubeViewer
{

CloudPointData::CloudPointData(const OfxPointI& size, OfxTime time)
    : _averageColor(time) // Selection average constructor (current time is needed)
{
    _size = size; // define first size (current clip number of pixels)
    _time = time; // get current time

    // cloud point VBO
    int imgSize = size.x * size.y; // number of pixel in the image
    _imgCopy.reserve(imgSize * 0.5); // reserve memory for buffer

    // selection buffer
    _selectionCopy.reserve(imgSize * 0.5); // reserve memory for color selection buffer
    _spillCopy.reserve(imgSize * 0.5); // reserve memory for spill selection buffer

    // VBO are not built at this time
    _isVBOBuilt = false; // cloud point VBO is not built
    _isSelectionVBOBuilt = false; // color selection VBO is not built
    _isSpillSelectionVBOBuilt = false; // spill selection VBO is not built
}

/**
 * Open src clip return if opening has been done (y or n)
 * @param clipSrc	source of the plugin
 * @param renderScale	current renderScale
 */
bool CloudPointData::generateVBOData(OFX::Clip* clipSrc, const OfxPointD& renderScale, const bool vboWithDiscretization,
                                     const int discretizationStep)
{
    _isVBOBuilt = false; // VBO is not built anymore
    // connection test
    if(!clipSrc->isConnected())
    {
        return false;
    }
    boost::scoped_ptr<OFX::Image> src(
        clipSrc->fetchImage(_time, clipSrc->getCanonicalRod(_time, renderScale))); // scoped pointer of current source clip
    // Compatibility tests
    if(!src.get()) // source isn't accessible
    {
        std::cout << "src is not accessible (cloud point)" << std::endl;
        return false;
    }
    if(src->getRowDistanceBytes() == 0) // if source is wrong
    {
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        return false;
    }
    const OfxRectI srcPixelRod = clipSrc->getPixelRod(_time, renderScale); // get current RoD
    if((clipSrc->getPixelDepth() != OFX::eBitDepthFloat) || (clipSrc->getPixelComponents() == OFX::ePixelComponentNone))
    {
        BOOST_THROW_EXCEPTION(exception::Unsupported()
                              << exception::user() + "Can't compute histogram data with the actual input clip format.");
        return false;
    }

    if(srcPixelRod != src->getBounds()) // the host does bad things !
    {
        // remove overlay... but do not crash.
        TUTTLE_LOG_WARNING("Image RoD and image bounds are not the same (rod=" << srcPixelRod
                                                                               << " , bounds:" << src->getBounds() << ").");
        return false;
    }
    // Compute if source is OK
    SView srcView = tuttle::plugin::getGilView<SView>(src.get(), srcPixelRod,
                                                      eImageOrientationIndependant); // get current view from source clip

    _imgCopy.clear(); // clear buffer
    if(vboWithDiscretization) // does user want to discretize the VBO
    {
        generateDiscretizedVBOData(srcView, discretizationStep); // create data and return buffer size
    }
    else
    {
        generateAllPointsVBOData(srcView); // create data and return buffer size
    }
    _isVBOBuilt = true; // VBO has been built
    return true;
}

/**
 * create the VBO from VBO data (draw function)
 */
void CloudPointData::updateVBO()
{
    // point cloud VBO
    _imgVBO.createVBO(&(_imgCopy.front()), _imgCopy.size() / 3); // generate VBO to draw
    _imgVBO._color = true; // activate color for VBO
    // color selection VBO
    _selectionColorVBO._colorDifferent = true; // color buffer is not the same than vertex buffer
    _selectionColorVBO._color = false; // disable color for VBO
    _selectionColorVBO.createVBO(&(_selectionCopy.front()), _selectionCopy.size() / 3, GL_STATIC_DRAW,
                                 &(_selectionCopy.front())); // generate color selection VBO to draw
    // spill selection VBO
    _selectionSpillVBO._colorDifferent = true; // color buffer is not the same than vertex buffer
    _selectionSpillVBO._color = false; // disable color for VBO
    _selectionSpillVBO.createVBO(&(_spillCopy.front()), _spillCopy.size() / 3, GL_STATIC_DRAW,
                                 &(_spillCopy.front())); // generate spill selection VBO to draw
}

/*
 * Copy RGB channels of the clip source into a buffer
 */
int CloudPointData::generateAllPointsVBOData(SView srcView)
{
    // compute buffer size
    int size = (int)(srcView.height() * srcView.width()); // return size : full image here

    // copy full image into buffer
    Pixel_copy funct(_imgCopy); // functor declaration
    // treatment
    terry::algorithm::transform_pixels(srcView, funct); // transform pixel did with functor reference
    return size;
}

/*
 * Copy discretization RGB channels of the clip source into a buffer
 */
int CloudPointData::generateDiscretizedVBOData(SView srcView, const int& discretizationStep)
{
    // compute buffer size
    int size = (int)(srcView.height() * srcView.width()); // return size : full image here

    // Create and use functor to get discretize data  (functor with template)
    Pixel_copy_discretization<SPixel> funct(_imgCopy, discretizationStep); // functor declaration
    terry::algorithm::transform_pixels(srcView, funct); // with functor reference
    funct.convertSetDataToVectorData(); // copy functor data to _imgCopy data
    size = _imgCopy.size(); // change size
    return size;
}

/*
 *
 */
bool CloudPointData::generateColorSelectionVBO(OFX::Clip* clipColor, const OfxPointD& renderScale,
                                               bool vboWithDiscretization, int discretizationStep)
{
    _isSelectionVBOBuilt = false; // selection VBO is not built
    // connection test
    if(!clipColor->isConnected())
    {
        return false;
    }

    boost::scoped_ptr<OFX::Image> src(clipColor->fetchImage(
        _time, clipColor->getCanonicalRod(_time, renderScale))); // scoped pointer of current color clip

    // Compatibility tests
    if(!src.get()) // color clip source isn't accessible
    {
        std::cout << "src is not accessible (color clip)" << std::endl;
        return false;
    }
    if(src->getRowDistanceBytes() == 0) // if source is wrong
    {
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        return false;
    }
    const OfxRectI srcPixelRod = clipColor->getPixelRod(_time, renderScale); // get current RoD
    if((clipColor->getPixelDepth() != OFX::eBitDepthFloat) || (clipColor->getPixelComponents() == OFX::ePixelComponentNone))
    {
        BOOST_THROW_EXCEPTION(exception::Unsupported()
                              << exception::user() + "Can't compute histogram data with the actual input clip format.");
        return false;
    }
    if(srcPixelRod != src->getBounds()) // the host does bad things !
    {
        // remove overlay... but do not crash.
        TUTTLE_LOG_WARNING("Image RoD and image bounds are not the same (rod=" << srcPixelRod
                                                                               << " , bounds:" << src->getBounds() << ").");
        return false;
    }
    // Compute if source is OK
    SView srcView = tuttle::plugin::getGilView<SView>(src.get(), srcPixelRod,
                                                      eImageOrientationIndependant); // get current view from source clip

    if(vboWithDiscretization) // there is discretization on VBO
    {
        // treatment VBO discretization (maybe)
    }
    // VBO without discretization
    generateAllPointsSelectionVBOData(srcView); // generate a selection VBO without discretization

    _isSelectionVBOBuilt = true; // selection VBO is not built
    return true;                 // treatment has been done correctly
}

/*
 * Copy RGB channels of the selected pixels in clip source into a buffer
 */
int CloudPointData::generateAllPointsSelectionVBOData(SView srcView)
{
    // compute buffer size
    int size; // returned size
    bool isSelection = true; // current operations are on selected pixels

    // clear selection copy
    _selectionCopy.clear(); // clear selection VBO data

    // copy full image into buffer
    Pixel_copy funct(_selectionCopy, isSelection); // functor declaration creation
    // treatment
    terry::algorithm::transform_pixels(srcView, funct); // transform pixel did with functor reference
    size = _selectionCopy.size(); // get current size of VBO

    return size; // return size of VBO buffers (same color and vertex)
}

/*
 *
 */
bool CloudPointData::generateSpillSelectionVBO(OFX::Clip* clipSpill, const OfxPointD& renderScale,
                                               bool vboWithDiscretization, int discretizationStep)
{
    _isSpillSelectionVBOBuilt = false; // selection VBO is not built
    // connection test
    if(!clipSpill->isConnected())
    {
        return false;
    }

    boost::scoped_ptr<OFX::Image> src(clipSpill->fetchImage(
        _time, clipSpill->getCanonicalRod(_time, renderScale))); // scoped pointer of current color clip

    // Compatibility tests
    if(!src.get()) // color clip source isn't accessible
    {
        std::cout << "src is not accessible (spill clip)" << std::endl;
        return false;
    }
    if(src->getRowDistanceBytes() == 0) // if source is wrong
    {
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        return false;
    }
    const OfxRectI srcPixelRod = clipSpill->getPixelRod(_time, renderScale); // get current RoD
    if((clipSpill->getPixelDepth() != OFX::eBitDepthFloat) || (clipSpill->getPixelComponents() == OFX::ePixelComponentNone))
    {
        BOOST_THROW_EXCEPTION(exception::Unsupported()
                              << exception::user() + "Can't compute histogram data with the actual input clip format.");
        return false;
    }
    if(srcPixelRod != src->getBounds()) // the host does bad things !
    {
        // remove overlay... but do not crash.
        TUTTLE_LOG_WARNING("Image RoD and image bounds are not the same (rod=" << srcPixelRod
                                                                               << " , bounds:" << src->getBounds() << ").");
        return false;
    }
    // Compute if source is OK
    SView srcView = tuttle::plugin::getGilView<SView>(src.get(), srcPixelRod,
                                                      eImageOrientationIndependant); // get current view from source clip

    if(vboWithDiscretization) // there is discretization on VBO
    {
        // treatment VBO discretization (maybe)
    }
    // VBO without discretization
    generateAllPointsSpillVBOData(srcView); // generate a selection VBO without discretization
    _isSpillSelectionVBOBuilt = true; // selection VBO is not built
    return true;                      // treatment has been done correctly
}

/*
 * Copy RGB channels of the selected pixels in clip source into a buffer
 */
int CloudPointData::generateAllPointsSpillVBOData(SView srcView)
{
    // compute buffer size
    int size; // returned size
    bool isSelection = true; // current operations are on selected pixels

    // clear selection copy
    _spillCopy.clear(); // clear selection VBO data

    // copy full image into buffer
    Pixel_copy funct(_spillCopy, isSelection); // functor declaration creation
    // treatment
    terry::algorithm::transform_pixels(srcView, funct); // transform pixel did with functor reference

    size = _spillCopy.size(); // get current size of VBO
    return size; // return size of VBO buffers (same color and vertex)
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
void CloudPointData::VBO::createVBO(const void* data, int size, GLenum usage, const void* dataColor)
{
    _size = size;
    genBuffer(_id, data, size, GL_ARRAY_BUFFER, usage);
}

/**
 * destroy a VBO
 * If VBO id is not valid or zero, then OpenGL ignores it silently.
 */
void CloudPointData::VBO::deleteVBO()
{
    if(_id != 0) // if VBO exists
    {
        glDeleteBuffers(1, &_id);
        _id = 0; // 0 is reserved, glGenBuffersARB() will return non-zero id if success
    }
    if(_idColor != 0)
    {
        glDeleteBuffers(1, &_idColor); // delete color buffer
        _idColor = 0; // reset color id
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
void CloudPointData::VBO::genBuffer(unsigned int& id, const void* data, int size, GLenum target, GLenum usage)
{
    // Test if buffer is not already existing
    if(id != 0) // if id is not null
        deleteVBO(); // delete current VBO

    const int dataSize = size * 3 * sizeof(float); // current size of data

    // Buffer creation
    glGenBuffers(1, &(_id));                     // create a VBO
    glBindBuffer(target, _id);                   // activate VBO id to use
    glBufferData(target, dataSize, data, usage); // upload data to video card

    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
    if(dataSize != bufferSize) // mismatch between data size and input array (control)
    {
        deleteVBO();
        std::cout << "[createVBO()] Mismatch between Data size and input array" << std::endl;
    }
}

void CloudPointData::VBO::genBufferColor(unsigned int& idColor, const void* data, int size, GLenum target, GLenum usage)
{
    // test if current VBO is already existing
    if(idColor != 0) // if idColor is not null
        deleteVBO(); // delete current VBO

    const int dataSize = size * 3 * sizeof(float);

    glGenBuffers(1, &(_idColor));                // create a VBO
    glBindBuffer(target, _idColor);              // activate VBO id to use
    glBufferData(target, dataSize, data, usage); // upload data to video card

    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
    if(dataSize != bufferSize) // mismatch between data size and input array (control)
    {
        deleteVBO();
        std::cout << "[createVBO()] Mismatch between Data size and input array" << std::endl;
    }
}

/*
 * Draw the current VBO on screen
 */
void CloudPointData::VBO::draw()
{
    if(_id && _size)
    {
        // bind VBOs with IDs and set the buffer offsets of the bound VBOs
        // When buffer object is bound with its ID, all pointers in gl*Pointer()
        // are treated as offset instead of real pointer.
        glBindBuffer(GL_ARRAY_BUFFER, _id);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY); // enable vertex arrays

        if(_color) // draw vector using color
        {
            if(_colorDifferent) // color and vertex buffers are not the same
            {
                glBindBuffer(GL_ARRAY_BUFFER, _idColor); // bind new color buffer
            }
            glColorPointer(3, GL_FLOAT, 0, 0); // point buffer which is used for colors (same than vertex or not)
            glEnableClientState(GL_COLOR_ARRAY);
        }

        glDrawArrays(GL_POINTS, 0, _size);

        glDisableClientState(GL_VERTEX_ARRAY); // disable vertex arrays

        if(_color)
            glDisableClientState(GL_COLOR_ARRAY);

        // it is good idea to release VBOs with ID 0 after use.
        // Once bound with 0, all pointers in gl*Pointer() behave as real
        // pointer, so, normal vertex array operations are re-activated
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
}
}
}
