%include <tuttle/host/global.i>
%include <tuttle/host/INode.i>
%include <tuttle/host/ofx/OfxhException.i>


%{
#define SWIG_FILE_WITH_INIT
#include <tuttle/host/InputBufferWrapper.hpp>
%}

#ifndef WITHOUT_NUMPY

%include "wrappers/numpy.i"

%init
%{
import_array();
%}

%inline
%{
	struct Inputbuffer_python_customData
	{
		Inputbuffer_python_customData()
			: _funcObject(NULL)
			, _imgObject(NULL)
		{}
		
		~Inputbuffer_python_customData()
		{
			SWIG_PYTHON_THREAD_BEGIN_BLOCK;
			if( _funcObject != NULL )
				Py_DECREF( _funcObject );
			if( _imgObject != NULL )
				Py_DECREF( _imgObject );
			SWIG_PYTHON_THREAD_END_BLOCK;
		}
		
		void setImgObject( PyObject* imgObject )
		{
			SWIG_PYTHON_THREAD_BEGIN_BLOCK;
			if( _imgObject != NULL )
				Py_DECREF( _imgObject );
			_imgObject = imgObject;
			Py_INCREF( _imgObject );
			SWIG_PYTHON_THREAD_END_BLOCK;
		}
		
		PyObject* _funcObject;
		PyObject* _imgObject;
	};
	void inputbuffer_python_callback( OfxTime time, void* object, void** rawdata, int* width, int* height, int* rowSizeBytes )
	{
		SWIG_PYTHON_THREAD_BEGIN_BLOCK;
		Inputbuffer_python_customData& customData = *(Inputbuffer_python_customData*)object;
		
//		TUTTLE_LOG_TRACE( "inputbuffer_python_callback: " << (void*)object << ", time: " << time << ", width: " << width << ", height: " << height << ", customData._funcObject: " << (void*)customData._funcObject );
		PyObject* ret = PyObject_CallFunction( customData._funcObject, (char *)"d", time );
		if( (void *)ret == NULL )
		{
			TUTTLE_LOG_TRACE( "TuttleOFX InputBuffer Python callback failed" );
			PyErr_SetString( PyExc_RuntimeError, "TuttleOFX InputBuffer Python callback failed" );
			*rawdata = NULL;
			*width = 0;
			*height = 0;
			*rowSizeBytes = 0;
			return;
		}
		
		if( ! PyTuple_Check(ret) || PyTuple_Size(ret) != 4 )
		{
			TUTTLE_LOG_TRACE( "The python callback doesn't return a tuple with 4 values as expected. Aborting..." );
			*rawdata = NULL;
			*width = 0;
			*height = 0;
			*rowSizeBytes = 0;
			return;
		}
#if PY_MAJOR_VERSION < 3
		*rawdata = PyString_AsString( PyTuple_GetItem(ret, 0) );
#else
		*rawdata = PyBytes_AsString( PyTuple_GetItem(ret, 0) );
#endif
//		TUTTLE_LOG_VAR( TUTTLE_TRACE, *rawdata );
		*width = static_cast<int>( PyInt_AsLong( PyTuple_GetItem(ret, 1) ) );
//		TUTTLE_LOG_VAR( TUTTLE_TRACE, *width );
		*height = static_cast<int>( PyInt_AsLong( PyTuple_GetItem(ret, 2) ) );
//		TUTTLE_LOG_VAR( TUTTLE_TRACE, *height );
		*rowSizeBytes = static_cast<int>( PyInt_AsLong( PyTuple_GetItem(ret, 3) ) );
//		TUTTLE_LOG_VAR( TUTTLE_TRACE, *rowSizeBytes );
		
		customData.setImgObject( ret );
		SWIG_PYTHON_THREAD_END_BLOCK;
	}
	
	void inputbuffer_destroy_callback( void* object )
	{
//		TUTTLE_LOG_TRACE( "inputbuffer_destroy_callback, Py_DECREF: " << (void*)object );
		delete (Inputbuffer_python_customData*)object;
	}
	
	typedef void *PyFunc;
%}

%typemap(in) PyFunc
{
  $1 = $input;
}

%extend tuttle::host::InputBufferWrapper
{
	void setPyCallback( PyFunc object )
	{
		SWIG_PYTHON_THREAD_BEGIN_BLOCK;
		if( PyCallable_Check( (PyObject *)object ) )
		{
			//TUTTLE_LOG_TRACE( "inputbuffer, Py_INCREF: " << (void*)object );
			Py_INCREF( (PyObject *)object );
			Inputbuffer_python_customData* customData = new Inputbuffer_python_customData();
			customData->_funcObject = (PyObject *)object;
			$self->setCallback( inputbuffer_python_callback, customData, inputbuffer_destroy_callback );
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


%include <tuttle/host/InputBufferWrapper.hpp>

%clear (float* rawBuffer, int width, int height);
%clear (unsigned short* rawBuffer, int width, int height);
%clear (unsigned char* rawBuffer, int width, int height);

%clear (float* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned short* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned char* rawBuffer, int height, int width, int nbComponents);
	
#else
%include <tuttle/host/InputBufferWrapper.hpp>
#endif
