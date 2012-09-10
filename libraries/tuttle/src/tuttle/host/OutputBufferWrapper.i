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
	
