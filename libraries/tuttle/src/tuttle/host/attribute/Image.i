%include <tuttle/host/global.i>
%include <tuttle/common/ofx/imageEffect.i>
%include <tuttle/host/ofx/OfxhImage.i>

%include <boost_shared_ptr.i>
%include <typemaps.i>
%include <cdata.i>

%shared_ptr(tuttle::host::attribute::Image)

%{
#include <tuttle/host/attribute/Image.hpp>
%}

%include <tuttle/host/attribute/Image.hpp>

namespace tuttle {
namespace host {
namespace attribute {

%ignore getImage;


%extend Image
{
	%pythoncode
	{
		def getImage(self):
			bounds = self.getBounds()
			return (
					self.getVoidPixelData(),
					bounds.x2 - bounds.x1,
					bounds.y2 - bounds.y1,
					self.getRowBytes(),
					self.getBitDepth(),
					self.getComponentsType()
				)

		def getNumpyArray(self):
			import numpy
			(data, width, height, rowSizeBytes, bitDepth, components) = self.getImage()
			
			#print '(width, height, rowSizeBytes, bitDepth, components)', (width, height, rowSizeBytes, bitDepth, components)

			numpyBitDepth = None
			if bitDepth == eBitDepthUByte:
				numpyBitDepth = numpy.uint8
			elif bitDepth == eBitDepthUShort:
				numpyBitDepth = numpy.uint16
			elif bitDepth == eBitDepthFloat:
				numpyBitDepth = numpy.float32
			else:
				raise TypeError('Unrecognized bit depth')
			
			bufferSize = rowSizeBytes*height
			d = cdata(data, bufferSize)
			arraySize = width*height*self.getNbComponents()
			if rowSizeBytes != (width*self.getNbComponents()*self.getBitDepthMemorySize()):
				print 'rowSizeBytes:', rowSizeBytes
				print 'width:', width
				print 'self.getNbComponents():', self.getNbComponents()
				print 'self.getBitDepthMemorySize():', self.getBitDepthMemorySize()
				raise NotImplementedError('The image padding is unsupported in the pyTuttle binding.')

			#print 'numpyBitDepth:', numpyBitDepth
			#print 'len(data):', len(d)
			#print 'rowSizeBytes:', rowSizeBytes
			#print 'size: %dx%d' % (width, height)
			#print 'nbComponents():', self.getNbComponents()
			#print 'arraySize:', arraySize
			
			flatarray = numpy.fromstring( d, numpyBitDepth, arraySize )

			#print 'flatarray:', flatarray
			nArray = numpy.array( numpy.flipud( numpy.reshape( flatarray, ( height, width, self.getNbComponents() ) ) ) )
			#print 'nArray:', nArray
			return nArray

		def getNumpyImage(self):
			import Image
			return Image.fromarray(self.getNumpyArray())
	}
}

}
}
}
