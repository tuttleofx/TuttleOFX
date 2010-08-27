#ifndef _TUTTLE_HOST_NODE_HPP_
#define _TUTTLE_HOST_NODE_HPP_

#include "graph/ProcessOptions.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace host {

namespace attribute {
class Attribute;
}

class Node
{
public:
	enum ENodeType
	{
		eNodeTypeImageEffect,
		eNodeTypeParam,
		eNodeTypeGraph,
	};

public:
	Node() {}
	Node( const Node& e ) {}
	virtual ~Node() = 0;
	virtual Node*            clone() const              = 0;
	virtual const std::string&      getName() const            = 0;
	virtual const ENodeType  getNodeType() const = 0;

	virtual attribute::Attribute& getAttribute( const std::string& name ) = 0;
//	const attribute::Attribute& getAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getAttribute( name ); }
	virtual attribute::Attribute& getSingleInputAttribute() = 0;
	virtual const attribute::Attribute& getSingleInputAttribute() const = 0;

#ifndef SWIG
	virtual void connect( const Node&, attribute::Attribute& ) = 0;

	virtual void begin( graph::ProcessOptions& processOptions )                 = 0;
	virtual void preProcess1_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess1_finish( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess2_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess2_finish( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess3_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess3_finish( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess_infos( graph::ProcessInfos& nodeInfos ) const       = 0;
	virtual void process( graph::ProcessOptions& processOptions )               = 0;
	virtual void postProcess( graph::ProcessOptions& processOptions )           = 0;
	virtual void end( graph::ProcessOptions& processOptions )                   = 0;
#endif
};

}
}

#endif

