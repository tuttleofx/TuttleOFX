%include <tuttle/common/exceptions.i>

%{
#include <tuttle/common/utils/global.hpp>
%}

// SWIG 2.0 Documenation
// "C and C++ style comments may appear anywhere in interface files. In previous versions of SWIG, comments were used to generate documentation files. However, this feature is currently under repair and will reappear in a later SWIG release."
//%style before, skip=1, keep, chop_top=1, chop_bottom=1, chop_left=3, chop_right=0
%feature("autodoc", "3");
%include <tuttle/common/utils/global.hpp>

