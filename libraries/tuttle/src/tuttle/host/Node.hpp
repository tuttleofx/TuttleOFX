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

class ProcessNode
{
public:
	enum EProcessNodeType
	{
		eImageEffectNode,
		eParamNode,
		eGraphNode,
	};

public:
	ProcessNode() {}
	ProcessNode( const ProcessNode& e ) {}
	virtual ~ProcessNode() = 0;

	virtual ProcessNode*            clone() const              = 0;
	virtual const std::string&      getName() const            = 0;
	virtual const EProcessNodeType  getProcessNodeType() const = 0;
	virtual attribute::Attribute& getAttribute( const std::string& name ) = 0;
//	const attribute::Attribute& getAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getAttribute( name ); }
	virtual attribute::Attribute& getSingleInputAttribute() = 0;
	virtual const attribute::Attribute& getSingleInputAttribute() const = 0;

	virtual void connect( const ProcessNode&, attribute::Attribute& ) = 0;

	virtual void begin( graph::ProcessOptions& processOptions )                 = 0;
	virtual void preProcess_initialize( graph::ProcessOptions& processOptions ) = 0;
	virtual void preProcess_finish( graph::ProcessOptions& processOptions )     = 0;
	virtual void process( const graph::ProcessOptions& processOptions )         = 0;
	virtual void postProcess( graph::ProcessOptions& processOptions )           = 0;
	virtual void end( graph::ProcessOptions& processOptions )                   = 0;
};

}
}

#endif

