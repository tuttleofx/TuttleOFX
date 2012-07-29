%include <tuttle/host/global.i>
%include <std_list.i>
%include <std_string.i>

%{
#include <tuttle/host/NodeListArg.hpp>
%}

namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
%template(NodePtrVector) vector<tuttle::host::INode*>;
%template(NodePtrList) list<tuttle::host::INode*>;
}

%include <tuttle/host/NodeListArg.hpp>

