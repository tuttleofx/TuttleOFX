%include <tuttle/host/global.i>
%include <tuttle/common/ofx/imageEffect.i>
%include <tuttle/host/ofx/OfxhImage.i>

%include <boost_shared_ptr.i>
%include <typemaps.i>

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
					self.getCharPixelData(),
					bounds.x2 - bounds.x1,
					bounds.y2 - bounds.y1,
					self.getRowBytes(),
					self.getBitDepth(),
					self.getComponentsType()
				)

		def getNumpyImage(self):
			import numpy
			import Image
			(data, width, height, rowSizeBytes, bitDepth, components) = self.getImage()

			numpyBitDepth = None
			if bitDepth == eBitDepthUByte:
				numpyBitDepth = numpy.uint8
			elif bitDepth == eBitDepthUShort:
				numpyBitDepth = numpy.uint16
			elif bitDepth == eBitDepthFloat:
				numpyBitDepth = numpy.float32
			else:
				raise LogicError('Unrecognized bit depth')
			
			print 'len(data):', len(data)
			print 'rowSizeBytes*height:', rowSizeBytes*height
			
			flatarray = numpy.fromstring( data, numpyBitDepth, rowSizeBytes*height )
			outImage = numpy.array( numpy.flipud( numpy.reshape( flatarray, ( height, width, self.getNbComponents() ) ) ) )
			return Image.fromarray(outImage)
	}
}

}
}
}
