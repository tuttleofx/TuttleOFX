#ifndef _TUTTLE_PROCESSNODE_HPP_
#define _TUTTLE_PROCESSNODE_HPP_

#include "ProcessOptions.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace host {
namespace core {

class ProcessNode
{
public:
	enum EProcessNodeType{
		  eImageEffect = 0
	};

public:
	ProcessNode() {}
	ProcessNode( const ProcessNode& e ) {}
	virtual ~ProcessNode() = 0;

	virtual ProcessNode* 			clone() const   = 0;
	virtual const std::string&		getName() const = 0;
	virtual const EProcessNodeType 	getProcessNodeType() const = 0;

	virtual void connect(const ProcessNode & inNode) = 0;

	virtual void begin(const ProcessOptions & processOptions) = 0;
	virtual void preProcess_initialize(const ProcessOptions & processOptions) = 0;
	virtual void preProcess_finish(const ProcessOptions & processOptions) = 0;
	virtual void process(const ProcessOptions & processOptions) = 0;
	virtual void postProcess(const ProcessOptions & processOptions) = 0;
	virtual void end(const ProcessOptions & processOptions) = 0;
};

} // namespace core
} // namespace host
} // namespace tuttle

#endif

