%include <tuttle/host/global.i>

%include <boost_shared_ptr.i>
%include <std_list.i>
%include <std_string.i>


%{
#include <tuttle/host/ComputeOptions.hpp>
%}

%shared_ptr(tuttle::host::IProgressHandle)

namespace std {
%template(TimeRangeList) list<tuttle::host::TimeRange>;
}

namespace tuttle {
namespace host {

// IProgressHandle could be called from another C++ thread,
// if we use ThreadEnv to compute the graph.
// So we need to use the Python GIL.
%threadblock IProgressHandle;

// If we throw an exception from the director overload (in Python),
// we need to convert this Python exception into a C++ exception.
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
%feature("director") IProgressHandle;

}
}


%include <tuttle/host/ComputeOptions.hpp>

