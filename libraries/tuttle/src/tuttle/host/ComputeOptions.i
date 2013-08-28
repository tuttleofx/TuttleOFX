%include <tuttle/host/global.i>

%include <std_list.i>
%include <std_string.i>

%{
#include <tuttle/host/ComputeOptions.hpp>
%}

namespace std {
%template(TimeRangeList) list<tuttle::host::TimeRange>;
}

namespace tuttle {
namespace host {
%feature("director") IProgressHandle;
}
}

%include <tuttle/host/ComputeOptions.hpp>

