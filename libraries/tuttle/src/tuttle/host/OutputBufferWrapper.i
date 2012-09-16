%include <tuttle/host/global.i>
%include <tuttle/host/INode.i>
%include <tuttle/host/ofx/OfxhException.i>


%{
#define SWIG_FILE_WITH_INIT
#include <tuttle/host/OutputBufferWrapper.hpp>
%}

%include "wrappers/numpy.i"

%init %{
import_array();
%}

%inline %{
  typedef void *PyFunc;
  void outputbuffer_python_callback(OfxTime time, void* object, void* rawdata, int width, int height, int rowSizeBytes, tuttle::host::OutputBufferWrapper::EBitDepth bitDepth, tuttle::host::OutputBufferWrapper::EPixelComponent components, tuttle::host::OutputBufferWrapper::EField field) {
    PyObject *ret;
    ret = PyObject_CallFunction((PyObject *)object, (char *)"ds#iiiiii", time, rawdata, height*rowSizeBytes, width, height, rowSizeBytes, bitDepth, components, field);
    if ((void *)ret == NULL) {
      PyErr_SetString(PyExc_RuntimeError, "TuttleOFX OutputBuffer Python callback failed");
    }
  }
%}

%typemap(in) PyFunc {
  $1 = $input;
}

%extend tuttle::host::OutputBufferWrapper {
  void setPyCallback(PyFunc object) {
    if (PyCallable_Check((PyObject *)object)) {
      if ($self->getNode().getParam( "callbackCustomData" ).getStringValue().c_str() != NULL)
	Py_DECREF($self->getNode().getParam( "callbackCustomData" ).getStringValue().c_str());
      Py_INCREF((PyObject *)object);
      $self->setCallback(outputbuffer_python_callback, object);
    } else {
      PyErr_SetString(PyExc_RuntimeError, "Callback function must be a callable");
    }
  }
  void __del__() {
    if ($self->getNode().getParam( "callbackCustomData" ).getStringValue().c_str() != NULL)
      Py_DECREF($self->getNode().getParam( "callbackCustomData" ).getStringValue().c_str());

  }
};

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
	
