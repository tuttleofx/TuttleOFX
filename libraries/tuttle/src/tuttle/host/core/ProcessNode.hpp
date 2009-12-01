#ifndef _TUTTLE_PROCESSNODE_HPP_
#define _TUTTLE_PROCESSNODE_HPP_

#include <tuttle/host/ofx/Attribute.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace host {
namespace core {

class ProcessNode
{
public:
	ProcessNode() {}
	ProcessNode( const ProcessNode& e ) {}
	virtual ~ProcessNode() = 0;

	virtual ProcessNode*       clone() const   = 0;
	virtual const std::string& getName() const = 0;
	//		virtual const std::vector<Attribute*>& getAttributes() const = 0;
};

} // namespace core
} // namespace host
} // namespace tuttle

#endif

