%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhClipImage.i>

%{
#include <tuttle/host/ofx/attribute/OfxhClipImageSet.hpp>
%}

%include <tuttle/host/ofx/attribute/OfxhClipImageSet.hpp>

%extend tuttle::host::ofx::attribute::OfxhClipImageSet
{
	%pythoncode
	{
		def getClips(self):
			nb = self.getNbClips()
			return [self.getNthClip(i) for i in range(nb)]
	}
}

