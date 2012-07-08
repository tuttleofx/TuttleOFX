%include <tuttle/host/global.i>
%include <tuttle/host/memory/MemoryCache.i>
%include <std_vector.i>
%include <std_string.i>

%include <tuttle/host/global.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/Node.i>
%include <tuttle/host/ofx/OfxhException.i>
%include <tuttle/host/ofx/property/OfxhPropertyTemplate.i>
%include <tuttle/host/ofx/property/OfxhSet.i>
%include <tuttle/host/ofx/property/OfxhNotifyHook.i>
%include <tuttle/host/ofx/property/OfxhGetHook.i>
%include <tuttle/host/ofx/attribute/OfxhParamSet.i>
//%include <tuttle/host/ofx/attribute/OfxhClipImageSet.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
//%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectNodeBase.i>


%{
#define SWIG_FILE_WITH_INIT
#include <tuttle/host/InputBufferNode.hpp>
%}

%include "wrappers/numpy.i"

%init %{
import_array();
%}


%include <tuttle/host/InputBufferNode.hpp>


%extend tuttle::host::ImageBuffer
{
	std::string __str__() const
	{
		std::stringstream s;
		s << *self;
		return s.str();
	}
}

%apply (unsigned char* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned char* rawBuffer, int height, int width)};
%apply (unsigned short* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned short* rawBuffer, int height, int width)};
%apply (float* INPLACE_ARRAY2, int DIM1, int DIM2) {(float* rawBuffer, int height, int width)};

%apply (unsigned char* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(unsigned char* rawBuffer, int height, int width, int nbComponents)};
%apply (unsigned short* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(unsigned short* rawBuffer, int height, int width, int nbComponents)};
%apply (float* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) {(float* rawBuffer, int height, int width, int nbComponents)};

%{
namespace tuttle {
namespace host {
	template<typename T>
	ImageBuffer<T> private_convertArray2(
		T* rawBuffer,
		int height,
		int width )
	{
		return ImageBuffer<T>( rawBuffer, width, height, ImageBuffer<T>::ePixelComponentAlpha );
	}
	template<typename T>
	ImageBuffer<T> private_convertArray3(
		T* rawBuffer,
		int height,
		int width,
		int nbComponents )
	{
		switch( nbComponents )
		{
			case 1:
				return ImageBuffer<T>( rawBuffer, width, height, ImageBuffer<T>::ePixelComponentAlpha );
			case 3:
				return ImageBuffer<T>( rawBuffer, width, height, ImageBuffer<T>::ePixelComponentRGB );
			case 4:
				return ImageBuffer<T>( rawBuffer, width, height, ImageBuffer<T>::ePixelComponentRGBA );
			default:
				BOOST_THROW_EXCEPTION( exception::BadConversion()
					<< exception::user() + "Your array has " + nbComponents + " components. There is no corresponding pixel component into TuttleOFX." );
		}
	}
}
}
%}


namespace tuttle {
namespace host {
%template(ImageBufferUChar) ImageBuffer<unsigned char>;
%template(ImageBufferUShort) ImageBuffer<unsigned short>;
%template(ImageBufferFloat) ImageBuffer<float>;

%template(setImage) InputBufferNode::setImage<unsigned char>;
%template(setImage) InputBufferNode::setImage<unsigned short>;
%template(setImage) InputBufferNode::setImage<float>;

// don't use %template because grayImage is defined in the .i file
ImageBuffer<unsigned char> private_convertArray2( unsigned char* rawBuffer, int height, int width );
ImageBuffer<unsigned short> private_convertArray2( unsigned short* rawBuffer, int height, int width );
ImageBuffer<float> private_convertArray2( float* rawBuffer, int height, int width );

ImageBuffer<unsigned char> private_convertArray3( unsigned char* rawBuffer, int height, int width, int nbComponents );
ImageBuffer<unsigned short> private_convertArray3( unsigned short* rawBuffer, int height, int width, int nbComponents );
ImageBuffer<float> private_convertArray3( float* rawBuffer, int height, int width, int nbComponents );


%pythoncode
%{
	def private_convertArrayDim( numpyArray ):
		if numpyArray.ndim == 2:
			return private_convertArray2( numpyArray )
		if numpyArray.ndim == 3:
			return private_convertArray3( numpyArray )
		raise ValueError("Unsupported array dimension: " + numpyArray.ndim )

	def convertArray( numpyArray ):
		# If it's not a C contiguous array, need to do a copy.
		if numpyArray.flags.c_contiguous:
			return private_convertArrayDim( numpyArray )
		else:
			raise ValueError("Unsupported array: Non contiguous buffer." )
			# todo: This is wrong! C++ will point to temporary copy, which is released by python.
			#return private_convertArrayDim( numpyArray.copy() )
%}

}
}

/*
namespace std {
%template(UByteVector) vector<unsigned char>;
%template(UShortVector) vector<unsigned short>;
%template(FloatVector) vector<float>;
}
*/



%clear (float* rawBuffer, int width, int height);
%clear (unsigned short* rawBuffer, int width, int height);
%clear (unsigned char* rawBuffer, int width, int height);

%clear (float* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned short* rawBuffer, int height, int width, int nbComponents);
%clear (unsigned char* rawBuffer, int height, int width, int nbComponents);

