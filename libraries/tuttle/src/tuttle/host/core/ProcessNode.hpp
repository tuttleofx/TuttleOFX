#ifndef _TUTTLE_PROCESSNODE_HPP_
#define _TUTTLE_PROCESSNODE_HPP_

#include "ProcessOptions.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace host {
namespace core {

class ProcessNode;

class ProcessAttribute
{
public:
	virtual const std::string& getName() const = 0;
	virtual const ProcessNode& getNode() const = 0;
};

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
	virtual ProcessAttribute& getProcessAttribute( const std::string& name ) = 0;
//	const ProcessAttribute& getProcessAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getProcessAttribute( name ); }
	virtual ProcessAttribute& getSingleInputAttribute() = 0;
	virtual const ProcessAttribute& getSingleInputAttribute() const = 0;

	virtual void connect( const ProcessNode&, ProcessAttribute& ) = 0;

	virtual void begin( ProcessOptions& processOptions )                 = 0;
	virtual void preProcess_initialize( ProcessOptions& processOptions ) = 0;
	virtual void preProcess_finish( ProcessOptions& processOptions )     = 0;
	virtual void process( const ProcessOptions& processOptions )         = 0;
	virtual void postProcess( ProcessOptions& processOptions )           = 0;
	virtual void end( ProcessOptions& processOptions )                   = 0;
};

}
}
}

#endif

