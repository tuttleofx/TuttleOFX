%include <tuttle/host/global.i>
%include <tuttle/host/INode.i>
%include <tuttle/host/ofx/OfxhException.i>


%{
#define SWIG_FILE_WITH_INIT
#include <tuttle/host/OutputBufferWrapper.hpp>
%}

#ifndef WITHOUT_NUMPY

%include "wrappers/numpy.i"

%init
%{
import_array();
%}

%inline
%{
	typedef void *PyFunc;
	void outputbuffer_python_callback( OfxTime time, void* object, void* rawdata, int width, int height, int rowSizeBytes, tuttle::host::OutputBufferWrapper::EBitDepth bitDepth, tuttle::host::OutputBufferWrapper::EPixelComponent components, tuttle::host::OutputBufferWrapper::EField field )
	{
		SWIG_PYTHON_THREAD_BEGIN_BLOCK;
		//std::cout << "outputbuffer_python_callback: " << (void*)object << ", time: " << time << ", width: " << width << ", height: " << height << std::endl;
		PyObject* ret = PyObject_CallFunction( (PyObject *)object, (char *)"ds#iiiiii", time, rawdata, height*rowSizeBytes, width, height, rowSizeBytes, bitDepth, components, field );
		if( (void *)ret == NULL )
		{
			PyErr_SetString( PyExc_RuntimeError, "TuttleOFX OutputBuffer Python callback failed" );
		}
		SWIG_PYTHON_THREAD_END_BLOCK;
	}
	
	void outputbuffer_destroy_callback( void* object )
	{
		SWIG_PYTHON_THREAD_BEGIN_BLOCK;
		//std::cout << "outputbuffer_destroy_callback, Py_DECREF: " << (void*)object << std::endl;
		if( object != NULL )
			Py_DECREF( (PyObject*)object );
		SWIG_PYTHON_THREAD_END_BLOCK;
	}
%}

%typemap(in) PyFunc
{
  $1 = $input;
}

%extend tuttle::host::OutputBufferWrapper
{
	void setPyCallback( PyFunc object )
	{
		SWIG_PYTHON_THREAD_BEGIN_BLOCK;
		if( PyCallable_Check((PyObject *)object) )
		{
			//std::cout << "outputbuffer, Py_INCREF: " << (void*)object << std::endl;
			Py_INCREF( (PyObject *)object );
			$self->setCallback( outputbuffer_python_callback, object, outputbuffer_destroy_callback );
		}
		else
		{
			PyErr_SetString(PyExc_RuntimeError, "Callback function must be a callable");
		}
		SWIG_PYTHON_THREAD_END_BLOCK;
	}
	
}

%ignore setCallback;

%apply (unsigned char* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned char* rawBuffer, int height, int width)};
%apply (unsigned short* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned short* rawBuffer, int height, int width)};
%apply (float* INPLACE_ARRAY2, int DIM1, int DIM2) {(float* rawBuffer, int height, int width)};

%apply (unsigned char* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(unsigned char* rawBuffer, int height, int width, int nbComponents)};
%apply (unsigned short* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(unsigned short* rawBuffer, int height, int width, int nbComponents)};
%apply (float* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(float* rawBuffer, int height, int width, int nbComponents)};


%include <tuttle/host/OutputBufferWrapper.hpp>

%clear (float* rawBuffer, int width, int height);
%clear (unsigned short* rawBuffer, int width, int height);
%clear (unsigned char* rawBuffer, int width, int height);

%clear (float* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned short* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned char* rawBuffer, int height, int width, int nbComponents);

#else
%include <tuttle/host/OutputBufferWrapper.hpp>
#endif
